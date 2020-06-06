#include "JointTransform.hpp"
Model::JointTransform::JointTransform(const glm::vec3& newPosition, const glm::quat& newRotation) {
    this->position = newPosition;
    this->rotation = newRotation;
}

Model::JointTransform Model::JointTransform::interpolate(const Model::JointTransform &first,
                                                         const Model::JointTransform &second,
                                                         float progression) {
    auto pos = glm::mix(first.position, second.position, progression);
    auto rot = glm::slerp(first.rotation, second.rotation, progression);
    return JointTransform(pos, rot);
}
glm::mat4 Model::JointTransform::getLocalTransform() {
    glm::mat4 out (1.0f);
    out = glm::translate(out, position);
    out = out * glm::toMat4(rotation);
    return out;
}
const glm::vec3 &Model::JointTransform::getPosition() const {
    return position;
}
void Model::JointTransform::setPosition(const glm::vec3 &position) {
    JointTransform::position = position;
}
const glm::quat &Model::JointTransform::getRotation() const {
    return rotation;
}
void Model::JointTransform::setRotation(const glm::quat &rotation) {
    JointTransform::rotation = rotation;
}
