#include "Animation.hpp"
Model::Animation::Animation(double time, const std::vector<KeyFrame>& newFrames) {
    this->length = time;
    this->keyFrames = newFrames;
}
double Model::Animation::getLength() const {
    return length;
}
std::vector<Model::KeyFrame> Model::Animation::getKeyFrames() {
    return keyFrames;
}
