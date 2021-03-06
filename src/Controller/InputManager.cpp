#include "Controller/InputManager.hpp"

#include <iostream>

#include "Controller/Engine/Engine.hpp"

namespace Controller::Input {

    InputManager &InputManager::getInstance() {
        static auto instance = InputManager{};

        return instance;
    }

    InputData InputManager::ProcessInput(GLEQevent &event) {
        auto &engine = BlueEngine::Engine::get();
        // Handle input data from event
        InputData inputEvent;
        int width, height;
        glfwGetWindowSize(engine.window, &width, &height);

        static glm::vec2 currentMousePos{width / 2, height / 2};

        switch (event.type) {
            case GLEQ_KEY_PRESSED: {
                inputEvent.inputType = BLUE_InputType::KEY_PRESS;
            } break;
            case GLEQ_KEY_RELEASED: {
                inputEvent.inputType = BLUE_InputType::KEY_RELEASE;
            } break;
            case GLEQ_CURSOR_MOVED: {
                glm::vec2 prevMousePos{currentMousePos};

                currentMousePos.x = static_cast<float>(event.pos.x);
                currentMousePos.y = static_cast<float>(event.pos.y);

                inputEvent.inputType             = BLUE_InputType::MOUSE_MOTION;
                inputEvent.mouseMotionRelative.x =
                    static_cast<int>(currentMousePos.x - prevMousePos.x);
                inputEvent.mouseMotionRelative.y =
                    static_cast<int>(currentMousePos.y - prevMousePos.y);
                inputEvent.mouseMotionAbsolute.x = event.pos.x;
                inputEvent.mouseMotionAbsolute.y = event.pos.y;
            } break;
            case GLEQ_BUTTON_PRESSED: {
                inputEvent.inputType = BLUE_InputType::MOUSE_BUTTONDOWN;
            } break;
            case GLEQ_BUTTON_RELEASED: {
                inputEvent.inputType = BLUE_InputType::MOUSE_BUTTONUP;
            } break;
            case GLEQ_SCROLLED: {
                inputEvent.inputType        = BLUE_InputType::MOUSE_WHEEL;
                inputEvent.mouseWheelMotion = static_cast<float>(event.scroll.y);

            } break;
            case GLEQ_WINDOW_RESIZED: {
                inputEvent.inputType = BLUE_InputType::WINDOW_RESIZE;
                break;
            }
            default: break;
        }

        switch (inputEvent.inputType) {
            case BLUE_InputType::KEY_PRESS:
            case BLUE_InputType::KEY_RELEASE: {
                for (auto &itr : InputMap) {
                    if (itr.second == event.keyboard.key) {
                        inputEvent.inputAction = itr.first;
                    }
                }
            } break;

            case BLUE_InputType::MOUSE_BUTTONDOWN:
            case BLUE_InputType::MOUSE_BUTTONUP: {
                GLFW_KEY_0;
                if (event.mouse.button == GLFW_MOUSE_BUTTON_LEFT) {
                    inputEvent.inputAction = BLUE_InputAction::INPUT_ACTION_1;
                } else if (event.mouse.button == GLFW_MOUSE_BUTTON_RIGHT) {
                    inputEvent.inputAction = BLUE_InputAction::INPUT_ACTION_2;
                } else if (event.mouse.button == GLFW_MOUSE_BUTTON_MIDDLE) {
                    inputEvent.inputAction = BLUE_InputAction::INPUT_ACTION_3;
                }
            } break;
            default: {

            } break;
        }

        return inputEvent;
    }

    int InputManager::hashStringToGLFWKey(const std::string &value) const {
        for (auto &n : stringScancodePairs) {
            if (n.first == value) {
                return n.second;
            }
        }
        // unknown key
        return 0;
    }

    std::string InputManager::hashGLFWKeyToString(const int value) const {
        for (auto &n : stringScancodePairs) {
            if (n.second == value) {
                return n.first;
            }
        }
        return std::string("Unknown");
    }

    std::string InputManager::hashInputActionToString(const BLUE_InputAction &value) const {
        for (auto &n : stringActionPairs) {
            if (n.second == value) {
                return n.first;
            }
        }
        return std::string("Unknown");
    }
    void InputManager::bindKey(BLUE_InputAction action, int key) {
        for (auto &n : InputMap) {
            if (n.second == key) {
                n.second = GLFW_KEY_UNKNOWN;
            }
        }
        InputMap.at(action) = key;
    }

    void InputManager::resetKeyStates() {
        for (auto &n : KeyStates) {
            n = false;
        }
    }

    void InputManager::recordKeyStates(GLEQevent &event) {
        if (event.type == GLEQ_KEY_PRESSED) {
            KeyStates[event.keyboard.key] = true;
        } else if (event.type == GLEQ_KEY_RELEASED) {
            KeyStates[event.keyboard.key] = false;
        }
    }

    const int *InputManager::getKeyStates() {
        return KeyStates;
    }

    void InputManager::DefaultInputMap() {
        InputMap.at(BLUE_InputAction::INPUT_JUMP)          = GLFW_KEY_SPACE;
        InputMap.at(BLUE_InputAction::INPUT_MOVE_FORWARD)  = GLFW_KEY_W;
        InputMap.at(BLUE_InputAction::INPUT_MOVE_BACKWARD) = GLFW_KEY_S;
        InputMap.at(BLUE_InputAction::INPUT_MOVE_LEFT)     = GLFW_KEY_A;
        InputMap.at(BLUE_InputAction::INPUT_MOVE_RIGHT)    = GLFW_KEY_D;
        InputMap.at(BLUE_InputAction::INPUT_ACTION_1)      = GLFW_KEY_E;
        InputMap.at(BLUE_InputAction::INPUT_ACTION_2)      = GLFW_KEY_K;
        InputMap.at(BLUE_InputAction::INPUT_ACTION_3)      = GLFW_KEY_M;
        InputMap.at(BLUE_InputAction::INPUT_ACTION_4)      = GLFW_KEY_X;
        InputMap.at(BLUE_InputAction::INPUT_SPRINT)        = GLFW_KEY_LEFT_SHIFT;
        InputMap.at(BLUE_InputAction::INPUT_CROUCH)        = GLFW_KEY_LEFT_CONTROL;
        InputMap.at(BLUE_InputAction::INPUT_ESCAPE)        = GLFW_KEY_ESCAPE;
    }

    std::map<BLUE_InputAction, int> &InputManager::getInputMap() {
        return InputMap;
    }

    void InputManager::createEnumStringPairs() {
        stringScancodePairs.clear();
        stringScancodePairs.push_back(std::pair<std::string, int>("1", GLFW_KEY_1));
        stringScancodePairs.push_back(std::pair<std::string, int>("2", GLFW_KEY_2));
        stringScancodePairs.push_back(std::pair<std::string, int>("3", GLFW_KEY_3));
        stringScancodePairs.push_back(std::pair<std::string, int>("4", GLFW_KEY_4));
        stringScancodePairs.push_back(std::pair<std::string, int>("5", GLFW_KEY_5));
        stringScancodePairs.push_back(std::pair<std::string, int>("6", GLFW_KEY_6));
        stringScancodePairs.push_back(std::pair<std::string, int>("7", GLFW_KEY_7));
        stringScancodePairs.push_back(std::pair<std::string, int>("8", GLFW_KEY_8));
        stringScancodePairs.push_back(std::pair<std::string, int>("9", GLFW_KEY_9));
        stringScancodePairs.push_back(std::pair<std::string, int>("0", GLFW_KEY_0));
        stringScancodePairs.push_back(std::pair<std::string, int>("A", GLFW_KEY_A));
        stringScancodePairs.push_back(std::pair<std::string, int>("B", GLFW_KEY_B));
        stringScancodePairs.push_back(std::pair<std::string, int>("C", GLFW_KEY_C));
        stringScancodePairs.push_back(std::pair<std::string, int>("D", GLFW_KEY_D));
        stringScancodePairs.push_back(std::pair<std::string, int>("E", GLFW_KEY_E));
        stringScancodePairs.push_back(std::pair<std::string, int>("F", GLFW_KEY_F));
        stringScancodePairs.push_back(std::pair<std::string, int>("G", GLFW_KEY_G));
        stringScancodePairs.push_back(std::pair<std::string, int>("H", GLFW_KEY_H));
        stringScancodePairs.push_back(std::pair<std::string, int>("I", GLFW_KEY_I));
        stringScancodePairs.push_back(std::pair<std::string, int>("J", GLFW_KEY_J));
        stringScancodePairs.push_back(std::pair<std::string, int>("K", GLFW_KEY_K));
        stringScancodePairs.push_back(std::pair<std::string, int>("L", GLFW_KEY_L));
        stringScancodePairs.push_back(std::pair<std::string, int>("M", GLFW_KEY_M));
        stringScancodePairs.push_back(std::pair<std::string, int>("N", GLFW_KEY_N));
        stringScancodePairs.push_back(std::pair<std::string, int>("O", GLFW_KEY_O));
        stringScancodePairs.push_back(std::pair<std::string, int>("P", GLFW_KEY_P));
        stringScancodePairs.push_back(std::pair<std::string, int>("Q", GLFW_KEY_Q));
        stringScancodePairs.push_back(std::pair<std::string, int>("R", GLFW_KEY_R));
        stringScancodePairs.push_back(std::pair<std::string, int>("S", GLFW_KEY_S));
        stringScancodePairs.push_back(std::pair<std::string, int>("T", GLFW_KEY_T));
        stringScancodePairs.push_back(std::pair<std::string, int>("U", GLFW_KEY_U));
        stringScancodePairs.push_back(std::pair<std::string, int>("V", GLFW_KEY_V));
        stringScancodePairs.push_back(std::pair<std::string, int>("W", GLFW_KEY_W));
        stringScancodePairs.push_back(std::pair<std::string, int>("X", GLFW_KEY_X));
        stringScancodePairs.push_back(std::pair<std::string, int>("Y", GLFW_KEY_Y));
        stringScancodePairs.push_back(std::pair<std::string, int>("Z", GLFW_KEY_Z));
        stringScancodePairs.push_back(std::pair<std::string, int>("Space", GLFW_KEY_SPACE));
        stringScancodePairs.push_back(std::pair<std::string, int>("LSHIFT", GLFW_KEY_LEFT_SHIFT));
        stringScancodePairs.push_back(std::pair<std::string, int>("LCTRL", GLFW_KEY_LEFT_CONTROL));
        stringScancodePairs.push_back(std::pair<std::string, int>("TAB", GLFW_KEY_TAB));
        stringScancodePairs.push_back(std::pair<std::string, int>("LALT", GLFW_KEY_LEFT_ALT));
        stringScancodePairs.push_back(std::pair<std::string, int>("ESCAPE", GLFW_KEY_ESCAPE));

        stringActionPairs.push_back(std::pair<std::string, BLUE_InputAction>(
            "Move Forward", BLUE_InputAction::INPUT_MOVE_FORWARD));
        stringActionPairs.push_back(std::pair<std::string, BLUE_InputAction>(
            "Move Backward", BLUE_InputAction::INPUT_MOVE_BACKWARD));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Move Left", BLUE_InputAction::INPUT_MOVE_LEFT));
        stringActionPairs.push_back(std::pair<std::string, BLUE_InputAction>(
            "Move Right", BLUE_InputAction::INPUT_MOVE_RIGHT));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Jump", BLUE_InputAction::INPUT_JUMP));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Sprint", BLUE_InputAction::INPUT_SPRINT));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Crouch", BLUE_InputAction::INPUT_CROUCH));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Menu", BLUE_InputAction::INPUT_ESCAPE));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Action 1", BLUE_InputAction::INPUT_ACTION_1));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Action 2", BLUE_InputAction::INPUT_ACTION_2));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Action 3", BLUE_InputAction::INPUT_ACTION_3));
        stringActionPairs.push_back(
            std::pair<std::string, BLUE_InputAction>("Action 4", BLUE_InputAction::INPUT_ACTION_4));
    }

    const std::vector<std::pair<std::string, int>> &InputManager::getStringScancodePairs() const {
        return stringScancodePairs;
    }

    InputManager::~InputManager() {}

    void InputManager::populateInputMap() { // Populates Input map with all actions to allow mapping inputs to them

        InputMap.insert(std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_JUMP, GLFW_KEY_UNKNOWN));
        InputMap.insert(std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_MOVE_FORWARD,
                                                         GLFW_KEY_UNKNOWN));
        InputMap.insert(std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_MOVE_BACKWARD,
                                                         GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_MOVE_LEFT, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_MOVE_RIGHT, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_SPRINT, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_CROUCH, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_ESCAPE, GLFW_KEY_UNKNOWN));

        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_ACTION_1, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_ACTION_2, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_ACTION_3, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_ACTION_4, GLFW_KEY_UNKNOWN));

        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_LOOK_UP, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_LOOK_DOWN, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_LOOK_LEFT, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_LOOK_RIGHT, GLFW_KEY_UNKNOWN));
        InputMap.insert(
            std::pair<BLUE_InputAction, int>(BLUE_InputAction::INPUT_DEFAULT, GLFW_KEY_UNKNOWN));
    }

    InputManager::InputManager() {
        populateInputMap();
        DefaultInputMap();
        createEnumStringPairs();
        resetKeyStates();
    }
}
