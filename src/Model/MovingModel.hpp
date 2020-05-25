#pragma once
#include <memory>

#include "View/Renderer/Shader.hpp"
#include <glm/gtc/quaternion.hpp>

namespace Model {
    class MovingModel {
      public:
        MovingModel();
        void Draw(glm::mat4 projection, glm::mat4 view);
        void Update(double t, double dt);
        glm::vec3 position = glm::vec3(0, 0, 0);
        //Model model = Model("res/model/badboy.fbx", false);
        size_t modelID = 0;
      private:
        void SetRotation(glm::vec3 &orig, glm::vec3 &dest);
        std::vector<glm::mat4> transforms = {};

        std::unique_ptr<Shader> ourShader = nullptr;
        glm::vec3 scale = glm::vec3(1.5f, 1.5f, 1.5f);
        glm::quat rotation = glm::quat(glm::vec3(glm::radians(-90.0f), 0.0f, 0.0f));
        glm::quat resultRotation = {};
        std::vector<glm::vec3> positions = {};
        size_t going = 0;
        float speed = 100.0f;
    };
}

