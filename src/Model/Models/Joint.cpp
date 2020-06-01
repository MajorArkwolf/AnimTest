//
// Created by Arkwolf on 25/05/2020.
//

#include "Joint.hpp"
Model::Joint::Joint(int newIndex, const std::string &newString, glm::mat4 newlocalTransform) {
    this->index = newIndex;
    this->name = newString;
    this->localBindTransform = newlocalTransform;
    this->animatedTransform = glm::mat4(1.0f);
    this->inverseBindTransform = glm::mat4(1.0f);
}

void Model::Joint::addChild(Model::Joint child) {
    this->children.emplace_back(child);
}
glm::mat4 Model::Joint::getAnimatedTransform() const {
    return animatedTransform;
}
void Model::Joint::setAnimationTransform(const glm::mat4 &newAnimTransform) {
    animatedTransform = newAnimTransform;
}
glm::mat4 Model::Joint::getInverseBindTransform() const {
    return inverseBindTransform;
}
void Model::Joint::calcInverseBindTransform(glm::mat4 parentBindTransform) {
    glm::mat4 bindTransform = parentBindTransform * localBindTransform;
    inverseBindTransform = glm::inverse(bindTransform);
    for (Joint child : children) {
        child.calcInverseBindTransform(bindTransform);
    }
}
