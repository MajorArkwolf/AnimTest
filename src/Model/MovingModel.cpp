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
    std::vector<glm::mat4> temp = anim->animatedModel->getJointTransforms();
    ourShader->setBool("animated", true);
    ourShader->setMat4Array("jointTransforms", temp);
    ourShader->setMat4("model", math_model);
    ModelManager::Draw(modelID, ourShader.get());
}
Model::MovingModel::MovingModel() {
    ourShader = std::make_unique<Shader>(Shader("res/shader/vertshader.vs", "res/shader/fragshader.fs"));
    positions.emplace_back(500, 0, 10);
    positions.emplace_back(10, 0, 10);
    positions.emplace_back(10, 0, 500);
    positions.emplace_back(250, 0, 250);
    positions.emplace_back(500, 0, 500);
    positions.emplace_back(250, 0, 250);
    SetRotation(position, positions[going]);
    //modelID = ModelManager::GetModelID("res/model/Cyl_Anim.fbx");
    modelID = ModelManager::GetModelID("res/model/model.dae");
    auto &model = ModelManager::GetModel(modelID);
    anim = std::make_shared<Controller::Animator>();
    anim->animatedModel = &model;
    anim->queAnimation(&model.animationList.at(0));
}

void Model::MovingModel::SetRotation(glm::vec3 &orig, glm::vec3 &dest) {
    auto world = glm::vec3(0.0f, 0.0f, 1.0f);
    auto result = dest - orig;

    auto angle = glm::angle(glm::normalize(result), glm::normalize(world));

    resultRotation = glm::quat(glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f))) * rotation;
}

void Model::MovingModel::Update(double t, double dt) {
    anim->update(t, dt);
}
