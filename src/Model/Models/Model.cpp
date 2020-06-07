#include "Model.hpp"

#include <iostream>
#include "View/Renderer/OpenGL.hpp"
#include "Controller/Engine/Engine.hpp"

static inline glm::vec3 vec3_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); } // it's aiVector3D because assimp's texture coordinates use that
static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline aiMatrix4x4 aiMatrix4x4Compose(const aiVector3D &scaling,
                               const aiQuaternion &rotation,
                               const aiVector3D &position) {
    aiMatrix4x4 r;

    aiMatrix3x3 m = rotation.GetMatrix();

    r.a1 = m.a1 * scaling.x;
    r.a2 = m.a2 * scaling.x;
    r.a3 = m.a3 * scaling.x;
    r.a4 = position.x;

    r.b1 = m.b1 * scaling.y;
    r.b2 = m.b2 * scaling.y;
    r.b3 = m.b3 * scaling.y;
    r.b4 = position.y;

    r.c1 = m.c1 * scaling.z;
    r.c2 = m.c2 * scaling.z;
    r.c3 = m.c3 * scaling.z;
    r.c4 = position.z;

    r.d1 = 0.0;
    r.d2 = 0.0;
    r.d3 = 0.0;
    r.d4 = 1.0;

    return r;
}

Model::Model::Model(char *path, bool gamma = false) : gammaCorrection(gamma) {
    loadModel(path);
}

Model::Model::Model(const string& path, bool gamma = false) : gammaCorrection(gamma) {
    loadModel(path);
}

void Model::Model::Draw(Shader& shader) {
    for (auto &mesh : meshes) {
        mesh.Draw(shader);
    }
}

void Model::Model::loadModel(string const &path) {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                    aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_GenSmoothNormals );
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) // if is Not Zero
    {
        string error = importer.GetErrorString();
        std::cout << "ERROR::ASSIMP:: " << error << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
    LoadBonesandJoints(scene);
    for (auto &mesh : meshes) {
        mesh.LoadBoneWeights();
        mesh.MeshToGPU();
    }
}

void Model::Model::processNode(aiNode *node, const aiScene *scene) {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    // data to fill
    std::vector<Vertex> vertices = {};
    std::vector<unsigned int> indices = {};
    std::vector<TextureB> textures = {};

    // Walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex = {};
        glm::vec3 vector{
            0, 0,
            0}; // we declare a placeholder vector since assimp uses
                // its own vector class that doesn't directly convert
                // to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        if (mesh->HasPositions()) {
            vector.x        = mesh->mVertices[i].x;
            vector.y        = mesh->mVertices[i].y;
            vector.z        = mesh->mVertices[i].z;
            vertex.Position = vector;
        }
        // normals
        if (mesh->HasNormals()) {
            vector.x      = mesh->mNormals[i].x;
            vector.y      = mesh->mNormals[i].y;
            vector.z      = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x            = mesh->mTextureCoords[0][i].x;
            vec.y            = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        // tangent
        if (mesh->HasTangentsAndBitangents()) {
            vector.x       = mesh->mTangents[i].x;
            vector.y       = mesh->mTangents[i].y;
            vector.z       = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x         = mesh->mBitangents[i].x;
            vector.y         = mesh->mBitangents[i].y;
            vector.z         = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse
    // texture should be named as 'texture_diffuseN' where N is a sequential
    // number ranging from 1 to MAX_SAMPLER_NUMBER. Same applies to other
    // texture as the following list summarizes: diffuse: texture_diffuseN
    // specular: texture_specularN normal: texture_normalN

    // 1. diffuse maps
    std::vector<TextureB> diffuseMaps =
        loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<TextureB> specularMaps =
        loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<TextureB> normalMaps =
        loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<TextureB> heightMaps =
        loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

std::vector<TextureB> Model::Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            const string& typeName) {
    std::vector<TextureB> textures = {};
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str = {};
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (auto & j : textures_loaded) {
            if (std::strcmp(j.path.data(), str.C_Str()) == 0) {
                textures.push_back(j);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) { // if texture hasn't been loaded already, load it
            TextureB texture = {};
            texture.id   = BlueEngine::Engine::get().renderer.TextureFromFile(str.C_Str(), this->directory, false);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(
                texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}

void Model::Model::LoadBones(unsigned MeshIndex, const aiMesh* pMesh)
{
//    for (unsigned i = 0 ; i < pMesh->mNumBones; ++i) {
//        unsigned boneIndex = 0;
//        string boneName(pMesh->mBones[i]->mName.data);
//
//        if (boneMapping.find(boneName) == boneMapping.end()) {
//            boneIndex = numBones;
//            ++numBones;
//            Bone bi;
//            bones.push_back(bi);
//        }
//        else {
//            boneIndex = boneMapping[boneName];
//        }
//
//        boneMapping[boneName] = boneIndex;
//        boneInfo[boneIndex].BoneOffset = mat4_cast(pMesh->mBones[i]->mOffsetMatrix);
//
//        for (unsigned j = 0 ; j < pMesh->mBones[i]->mNumWeights; ++j) {
//            unsigned VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
//            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
//            meshes.at(MeshIndex).AddBoneData(VertexID, boneIndex, Weight);
//        }
//    }
}

void Model::Model::LoadBonesandJoints(const aiScene *scene) {
    // Find channels, and the bones used in the channels
    for (unsigned int ca = 0; ca < scene->mNumAnimations; ++ca) {
        animations.emplace_back();
        auto &animation = animations[ca];

        animation.duration       = scene->mAnimations[ca]->mDuration;
        animation.ticksPerSecond = scene->mAnimations[ca]->mTicksPerSecond;

        animation.channels.resize(scene->mAnimations[ca]->mNumChannels);
        for (unsigned int cc = 0; cc < scene->mAnimations[ca]->mNumChannels;
             ++cc) {
            animation.channels[cc].positions.resize(
                scene->mAnimations[ca]->mChannels[cc]->mNumPositionKeys);
            animation.channels[cc].scales.resize(
                scene->mAnimations[ca]->mChannels[cc]->mNumScalingKeys);
            animation.channels[cc].rotations.resize(
                scene->mAnimations[ca]->mChannels[cc]->mNumRotationKeys);

            for (unsigned int cp = 0;
                 cp < scene->mAnimations[ca]->mChannels[cc]->mNumPositionKeys;
                 cp++) {
                animation.channels[cc].positions[cp] =
                    scene->mAnimations[ca]->mChannels[cc]->mPositionKeys[cp];
            }
            for (unsigned int cs = 0;
                 cs < scene->mAnimations[ca]->mChannels[cc]->mNumScalingKeys;
                 cs++) {
                animation.channels[cc].scales[cs] =
                    scene->mAnimations[ca]->mChannels[cc]->mScalingKeys[cs];
            }
            for (unsigned int cr = 0;
                 cr < scene->mAnimations[ca]->mChannels[cc]->mNumRotationKeys;
                 cr++) {
                animation.channels[cc].rotations[cr] =
                    scene->mAnimations[ca]->mChannels[cc]->mRotationKeys[cr];
            }

            const aiNode *node = scene->mRootNode->FindNode(
                scene->mAnimations[ca]->mChannels[cc]->mNodeName);
            animations[ca].channels[cc].boneId = getBoneId(node);
        }
        for (unsigned int cm = 0; cm < scene->mNumMeshes; ++cm) {
            for (unsigned int cb = 0; cb < scene->mMeshes[cm]->mNumBones; cb++) {
                const aiNode *node = scene->mRootNode->FindNode(
                    scene->mMeshes[cm]->mBones[cb]->mName);
                this->meshes[cm].boneWeights.emplace_back();
                unsigned int boneId                     = getBoneId(node);
                this->meshes[cm].boneWeights[cb].boneId = boneId;
                this->meshes[cm].boneWeights[cb].offsetMatrix =
                    scene->mMeshes[cm]->mBones[cb]->mOffsetMatrix;

                for (unsigned int cw = 0;
                     cw < scene->mMeshes[cm]->mBones[cb]->mNumWeights; cw++) {
                    this->meshes[cm].boneWeights[cb].weights.emplace_back(
                        scene->mMeshes[cm]->mBones[cb]->mWeights[cw]);
                }

                if (!bones[boneId].hasParentBoneId) {
                    // Populate Bone::parentBoneIds
                    node = node->mParent;
                    while (node != scene->mRootNode) {
                        unsigned int parentBoneId     = getBoneId(node);
                        bones[boneId].parentBoneId    = parentBoneId;
                        bones[boneId].hasParentBoneId = true;
                        boneId                        = parentBoneId;

                        node = node->mParent;
                    }
                }
            }
        }
    }
}
// Add bone if it does not yet exist, and return boneId
unsigned int Model::Model::getBoneId(const aiNode *node) {
    unsigned int boneId;
    if (boneName2boneId.count(node->mName.data) == 0) {
        bones.emplace_back();
        boneId                            = bones.size() - 1;
        boneName2boneId[node->mName.data] = boneId;

        bones[boneId].transformation = node->mTransformation;
    } else {
        boneId = boneName2boneId[node->mName.data];
    }

    return boneId;
}

aiMatrix4x4 Model::Model::calculateBoneTransformation(unsigned int boneId) {
    aiMatrix4x4 transformation;
    if (bones[boneId].hasParentBoneId) {
        calculateBoneTransformation(bones[boneId].parentBoneId);
    }
    transformation *= bones[boneId].transformation
}

std::vector<glm::mat4> Model::Model::createFrame(unsigned int animationId, double time, bool loop = true) {
    std::vector<aiMatrix4x4> poses(1.0f);
    std::vector<glm::mat4> poses2(1.0f);
    poses.resize(50);
    poses2.resize(50);
    if (animationId < animations.size()) {
        for (auto &channel : animations[animationId].channels) {
            aiVector3D scale =
                animations[animationId].interpolate(channel.scales, time, loop);
            aiQuaternion rotation = animations[animationId].interpolate(
                channel.rotations, time, loop);
            aiVector3D position = animations[animationId].interpolate(
                channel.positions, time, loop);
//            bones[channel.boneId].transformation =
//                aiMatrix4x4Compose(scale, rotation, position);
            poses.at(channel.boneId) = aiMatrix4x4Compose(scale, rotation, position);
        }
    }
    for (size_t i = 0; i < 50; ++i) {
        poses2[i] = mat4_cast(poses[i]);
    }
    return poses2;
}
