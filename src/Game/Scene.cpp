#include "Scene.hpp"
#include "Controller/Engine/Engine.hpp"

using Controller::Input::BLUE_InputAction;
using Controller::Input::BLUE_InputType;

Scene::Scene() {
    camera.Position.y = 50.0;
//    this->tLoader.loadMaterialTextures("dirt.jpg");
//    this->tLoader.loadMaterialTextures("grass2.png");
//    auto list = this->tLoader.getTextureList();
//    this->terrain.setTextures(list.at(0).id, list.at(1).id);

//    mModel.model.animator = std::make_shared<Controller::Animator>(*mModel.model.rootJoint);
//    mModel.model.animator->setAnimation(mModel.model.animationList.at(0));
}
Scene::~Scene() = default;

void Scene::Draw() {
    int width = 0, height = 0;
    auto &engine      = BlueEngine::Engine::get();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwGetWindowSize(engine.window, &width, &height);
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom),
        static_cast<double>(width) / static_cast<double>(height), 0.1, 100000.0);
    glm::mat4 view = camera.GetViewMatrix();
    mModel.Draw(projection, view);
    //terrain.draw(projection, view);
    glfwSwapBuffers(engine.window);
}
void Scene::Update(double t, double dt) {
    if (moveForward) {
        camera.ProcessKeyboard(Camera_Movement::FORWARD, dt);
    }
    if (moveBackward) {
        camera.ProcessKeyboard(Camera_Movement::BACKWARD, dt);
    }
    if (moveLeft) {
        camera.ProcessKeyboard(Camera_Movement::LEFT, dt);
    }
    if (moveRight) {
        camera.ProcessKeyboard(Camera_Movement::RIGHT, dt);
    }
    mModel.Update(t, dt);
    //mModel.position.y = terrain.getBLHeight(mModel.position.x, mModel.position.z);
}

void Scene::handleInputData(Controller::Input::InputData inputData) {
    auto &engine      = BlueEngine::Engine::get();
    auto handledMouse = false;

    switch (inputData.inputType) {
        case BLUE_InputType::KEY_PRESS: { //  Key Press events

            switch (inputData.inputAction) {
                case BLUE_InputAction::INPUT_MOVE_FORWARD: {
                    moveForward = true;
                } break;
                case BLUE_InputAction::INPUT_MOVE_BACKWARD: {
                    moveBackward = true;
                } break;
                case BLUE_InputAction::INPUT_MOVE_LEFT: {
                    moveLeft = true;
                } break;
                case BLUE_InputAction::INPUT_MOVE_RIGHT: {
                    moveRight = true;
                } break;
                case BLUE_InputAction::INPUT_ESCAPE: {
                    engine.endEngine();
                } break;

                default: break;
            }

        } break;
        case BLUE_InputType::KEY_RELEASE: { // Key Release events
            switch (inputData.inputAction) {
                case BLUE_InputAction::INPUT_MOVE_FORWARD: {
                    moveForward = false;
                } break;
                case BLUE_InputAction::INPUT_MOVE_BACKWARD: {
                    moveBackward = false;
                } break;
                case BLUE_InputAction::INPUT_MOVE_LEFT: {
                    moveLeft = false;
                } break;
                case BLUE_InputAction::INPUT_MOVE_RIGHT: {
                    moveRight = false;
                } break;
                case BLUE_InputAction::INPUT_ACTION_2: {
                    auto &renderer = BlueEngine::Engine::get().renderer;
                    renderer.ToggleWireFrame();
                } break;
                case BLUE_InputAction::INPUT_ACTION_3: {
                    //guiManager.toggleWindow("instructions");
                } break;
                case BLUE_InputAction::INPUT_ACTION_4: {
                    engine.endEngine();
                } break;
                default: break;
            }
        } break;
        case BLUE_InputType::MOUSE_MOTION: { // Mouse motion event
            if (!engine.getMouseMode()) {
                auto x = static_cast<double>(inputData.mouseMotionRelative.x);
                auto y = static_cast<double>(inputData.mouseMotionRelative.y) * -1;
                camera.ProcessMouseMovement(x, y);
                handledMouse = true;
            }

        } break;
        case BLUE_InputType::MOUSE_WHEEL: { // Mouse Wheel event
            double amountScrolledY = static_cast<double>(inputData.mouseWheelMotion);
            camera.ProcessMouseScroll(amountScrolledY);
        } break;
        case BLUE_InputType::WINDOW_RESIZE: {
            engine.renderer.ResizeWindow();
        } break;
        default: break;
    }
    if (!handledMouse) {
        engine.mouse = {0.0f, 0.0f};
    }
}