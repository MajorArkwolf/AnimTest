
#include "Animator.hpp"
#include "Model/Models/JointTransform.hpp"
#include "Model/Models/Model.hpp"
void Controller::Animator::queAnimation(Model::Animation* newAnimation) {
    animationTime = 0;
    animation = newAnimation;
}
void Controller::Animator::update(double t, double dt) {
    if (animation == nullptr) {
        return;
    }
    increaseAnimationTime(dt);
    auto currentPose = calculateCurrentAnimationPose();
    glm::mat4 newMat(1.0f);
    applyPoseToJoints(currentPose, *animatedModel->rootJoint, newMat);
}
void Controller::Animator::increaseAnimationTime(double time) {
    animationTime += time;
    if (animationTime > animation->getLength()) {
        animationTime = fmod(animationTime, animation->getLength());
    }
}
std::map<std::string, glm::mat4> Controller::Animator::calculateCurrentAnimationPose() {
    std::vector<Model::KeyFrame> frames = getPreviousAndNextFrames();
    double progression = calculateProgression(frames[0], frames[1]);
    return interpolatePoses(frames[0], frames[1], progression);
}
void Controller::Animator::applyPoseToJoints(std::map<std::string, glm::mat4>& currentPose,
                                             Model::Joint& joint, glm::mat4 parentTransform) {
//    glm::mat4 currentLocalTransform = currentPose.at(joint.name);
//    glm::mat4 currentTransform = parentTransform * currentLocalTransform;
//    for (Model::Joint& childJoint : joint.children) {
//        applyPoseToJoints(currentPose, childJoint, currentTransform);
//    }
//    currentTransform = currentTransform * joint.getInverseBindTransform();
//    joint.setAnimationTransform(currentTransform);
    glm::mat4 NodeTransformation;
    if (currentPose.count(joint.name)) {
        NodeTransformation = currentPose.at(joint.name);
    } else {
        NodeTransformation = glm::mat4(1.0f);
    }

    glm::mat4 GlobalTransformation = parentTransform * NodeTransformation;
    if (animatedModel->boneMapping.find(joint.name) != animatedModel->boneMapping.end()) {
        unsigned BoneIndex = animatedModel->boneMapping[joint.name];
        joint.setAnimationTransform(animatedModel->globalInverseTransform * GlobalTransformation *
                                                                 animatedModel->boneInfo[BoneIndex].BoneOffset);
    }
    for (Model::Joint& childJoint : joint.children) {
        applyPoseToJoints(currentPose, childJoint, GlobalTransformation);
    }

}

std::vector<Model::KeyFrame> Controller::Animator::getPreviousAndNextFrames() {
    std::vector<Model::KeyFrame> allFrames = animation->getKeyFrames();
    Model::KeyFrame previousFrame = allFrames[0];
    Model::KeyFrame nextFrame = allFrames[0];
    for (int i = 1; i < allFrames.size(); i++) {
        nextFrame = allFrames[i];
        if (nextFrame.getTimeStamp() > animationTime) {
            break;
        }
        previousFrame = allFrames[i];
    }
    return std::vector<Model::KeyFrame> { previousFrame, nextFrame };
}
double Controller::Animator::calculateProgression(const Model::KeyFrame& previousFrame,
                                                 const Model::KeyFrame& nextFrame) {
    double totalTime = nextFrame.getTimeStamp() - previousFrame.getTimeStamp();
    double currentTime = animationTime - previousFrame.getTimeStamp();
    return currentTime / totalTime;
}
std::map<std::string, glm::mat4> Controller::Animator::interpolatePoses(Model::KeyFrame& previousFrame,
                                                                        Model::KeyFrame& nextFrame,
                                                                        double progression) {
    std::map<std::string, glm::mat4> currentPose = {};
    for (auto& jointName : previousFrame.getJointKeyFrames()) {
        Model::JointTransform previousTransform = previousFrame.getJointKeyFrames().at(jointName.first);
        Model::JointTransform nextTransform = nextFrame.getJointKeyFrames().at(jointName.first);
        Model::JointTransform currentTransform = Model::JointTransform::interpolate(previousTransform, nextTransform, static_cast<float>(progression));
        currentPose.emplace(jointName.first, currentTransform.getLocalTransform());
    }
    return currentPose;
}
