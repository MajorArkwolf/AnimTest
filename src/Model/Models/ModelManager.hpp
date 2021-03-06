#pragma once
#include <vector>
#include "Model/Models/Model.hpp"
#include "View/Renderer/Shader.hpp"

class ModelManager {
  public:
    static auto ModelRepo() -> std::vector<Model::Model> &;
    static auto GetModelID(const std::string& filename) -> size_t;
    static void Draw(size_t id, Shader *ourShader);

    friend class ResourceManager;
    static Model::Model& GetModel(size_t index);
};
