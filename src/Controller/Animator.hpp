#pragma once
#include <string>
#include <map>
#include "Model/Models/Animation.hpp"
#include "Model/Models/Joint.hpp"
namespace Model {
    class Model;

}

namespace Controller {
    class Animator {
      public:
        Model::Model *animatedModel = nullptr;
        Model::Animation *animation = nullptr;
        double animationTime = 0;
        Animator() = default;
        void queAnimation(Model::Animation* newAnimation);
        void update(double t, double dt);
        void increaseAnimationTime(double time);
        std::map<std::string, glm::mat4> calculateCurrentAnimationPose();
        void applyPoseToJoints(std::map<std::string, glm::mat4>& currentPose, Model::Joint& joint, glm::mat4 parentTransform);
        std::vector<Model::KeyFrame> getPreviousAndNextFrames();
        double calculateProgression(const Model::KeyFrame& previousFrame, const Model::KeyFrame& nextFrame);
        std::map<std::string, glm::mat4> interpolatePoses(Model::KeyFrame& previousFrame, Model::KeyFrame& nextFrame, double progression);
    };
}

