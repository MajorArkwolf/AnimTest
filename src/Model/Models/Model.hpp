#pragma once
#include <string>
#include <vector>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.hpp"
#include "View/Renderer/Shader.hpp"
#include "Model/Models/Joint.hpp"
#include "Model/Models/Animation.hpp"
namespace Model {
    class Model {
      public:
        /// Textures IDs that have been loaded.
        std::vector<TextureB> textures_loaded = {}; // Stores all textures, needs to be global
        /// Meshes of models.
        std::vector<Mesh> meshes = {};
        /// The directory of the model.
        std::string directory = {};
        /// Does the model need gamma correction.
        bool gammaCorrection = {};
        std::vector<BoneInfo> boneInfo = {};
        std::map<std::string, unsigned int> boneMapping = {};
        glm::mat4 globalInverseTransform = {};
        int numBones = 0;
        std::shared_ptr<Joint> rootJoint = nullptr;
        std::vector<Animation> animationList = {};

        /**
         * Constructor for the model.
         * @param path path to the model
         * @param gamma correction if required.
         */
        Model(char *path, bool gamma);
        /**
         * Constructor for the model.
         * @param path path to the model
         * @param gamma correction if required.
         */
        Model(const std::string& path, bool gamma);

        /**
         * Draw call for the model
         * @param shader used to draw the model.
         */
        void Draw(Shader& shader);

        std::vector<glm::mat4> getJointTransforms();

      private:
        /**
         * Loads a model from file.
         * @param path to the model.
         */
        void loadModel(const std::string &path);
        /**
         * Process Nodes from ASSIMP into the models.
         * @param node of the model.
         * @param scene the model loaded in.
         */
        void processNode(aiNode *node, const aiScene *scene);

        /**
         * Process the mesh of a model.
         * @param mesh of the model.
         * @param scene scene the model loaded in.
         * @return returns the next mesh.
         */
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<TextureB> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  const std::string& typeName);

        void LoadBones(unsigned int MeshIndex, const aiMesh *pMesh);
        void LoadJoints(aiMesh *mesh, const aiScene *scene);
        Joint RecurseJoints(aiNode* parent, const aiScene *scene);
        void LoadAnimation(const aiScene *scene);
        void addJointsToArray(Joint &headJoint, std::vector<glm::mat4> &jointMatrices);

    };
}