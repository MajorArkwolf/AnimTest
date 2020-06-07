#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "MovingModel.hpp"
#include <iostream>
#include "Model/Models/ModelManager.hpp"

void Model::MovingModel::Draw(glm::mat4 projection, glm::mat4 view) {
    ourShader->use();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    // render the loaded model
    glm::mat4 math_model = glm::mat4(1.0f);
    math_model = glm::translate(math_model, position); // translate it down so it's at the center of the scene
    math_model = glm::scale(math_model, scale);	// it's a bit too big for our scene, so scale it down
    math_model *= glm::toMat4(resultRotation);
    std::vector<glm::mat4> temp = ModelManager::ModelRepo().at(modelID).createFrame(0, 1, true);
    ourShader->setBool("animated", true);
    ourShader->setMat4Array("jointTransforms", temp);
    ourShader->setMat4("model", math_model);
    ModelManager::Draw(modelID, ourShader.get());
}
Model::MovingModel::MovingModel() {
    ourShader = std::make_unique<Shader>(Shader("res/shader/vertshader.vs", "res/shader/fragshader.fs"));
    modelID = ModelManager::GetModelID("res/model/model.dae");
}

void Model::MovingModel::SetRotation(glm::vec3 &orig, glm::vec3 &dest) {
    auto world = glm::vec3(0.0f, 0.0f, 1.0f);
    auto result = dest - orig;

    auto angle = glm::angle(glm::normalize(result), glm::normalize(world));

    resultRotation = glm::quat(glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f))) * rotation;
}

void Model::MovingModel::Update(double t, double dt) {
    //model.Update(static_cast<float>(t), static_cast<float>(dt));
//    animator.BoneTransform(static_cast<float>(t));
//    auto totalSpeed = speed * static_cast<float>(dt);
//    glm::vec3 result = glm::normalize((positions[going] - position) * totalSpeed);
//    position += result;
//
//    if (floor(position.x) == positions[going].x || ceil(position.x) == positions[going].x) {
//        if (floor(position.z) == positions[going].z || ceil(position.z) == positions[going].z) {
//            ++going;
//            if (going >= positions.size()) {
//                going = 0;
//            }
//            SetRotation(position, positions[going]);
//        }
//    }
}
