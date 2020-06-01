#pragma once
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
namespace Model {
    class JointTransform {
      private:
        glm::vec3 position = {};
        glm::quat rotation = {};

      public:
        JointTransform(const glm::vec3& newPosition, const glm::quat& newRotation);
        JointTransform() = default;
        glm::mat4 getLocalTransform();
        static JointTransform interpolate(const JointTransform& first, const JointTransform& second, float progression);
        const glm::vec3 &getPosition() const;
        void setPosition(const glm::vec3 &position);
        const glm::quat &getRotation() const;
        void setRotation(const glm::quat &rotation);
    };
}
