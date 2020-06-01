//
// Created by Arkwolf on 25/05/2020.
//

#include "KeyFrame.hpp"
Model::KeyFrame::KeyFrame(double newTimeStamp, const std::map<std::string, JointTransform>& newPose) {
    this->timeStamp = newTimeStamp;
    this->pose = newPose;
}

Model::KeyFrame::KeyFrame(double newTimeStamp) {
    this->timeStamp = newTimeStamp;
    this->pose = {};
}

double Model::KeyFrame::getTimeStamp() const {
    return timeStamp;
}
std::map<std::string, Model::JointTransform> &Model::KeyFrame::getJointKeyFrames() {
    return pose;
}
