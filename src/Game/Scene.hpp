#pragma once
#include <vector>
#include "Controller/InputManager.hpp"
#include "View/EulerCamera.hpp"
#include "View/Renderer/Shader.hpp"
#include "Model/MovingModel.hpp"


class Scene {
  public:
    Scene();
    ~Scene();
    void Draw();
    void Update(double t, double dt);
    void handleInputData(Controller::Input::InputData inputData);
  private:
    bool moveForward = false, moveBackward = false, moveLeft = false, moveRight = false;
    Model::MovingModel mModel = {};
    View::Camera camera = {};
};

