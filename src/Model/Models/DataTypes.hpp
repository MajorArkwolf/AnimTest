#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>


constexpr unsigned NUM_BONES_PER_VEREX = 4;

#include <vector>

#include <assimp/matrix4x4.h>
#include <assimp/mesh.h>

struct Vertex {
    glm::vec3 Position = {};
    glm::vec3 Bitangent = {};
    glm::vec3 Tangent = {};
    glm::vec2 Normal = {};
    glm::vec2 TexCoords = {};
    glm::ivec4 BoneIDs = {};
    glm::vec4 BoneWeight = {};
};

//struct BoneInfo
//{
//    glm::mat4 BoneOffset = glm::mat4(1.0f);
//    glm::mat4 FinalTransformation = glm::mat4(1.0f);
//};

struct TextureB {
    std::string type = {};
    std::string path = {};
    unsigned int id = 0;
};
