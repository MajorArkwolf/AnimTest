#pragma once
#include <vector>
#include <string>
#include <glm/matrix.hpp>


namespace Model {
    class Joint {
      public:
        int index = 0;
        std::string name = "";
        std::vector <Joint> children = {};
        glm::mat4 animatedTransform;
        glm::mat4 localBindTransform;
        glm::mat4 inverseBindTransform;

        Joint(int newIndex, const std::string& newString, glm::mat4 newlocalTransform);

        void addChild(Joint child);

        glm::mat4 getAnimatedTransform() const;
        void setAnimationTransform(const glm::mat4& newAnimTransform);
        glm::mat4 getInverseBindTransform() const;
        void calcInverseBindTransform(glm::mat4 parentBindTransform);
    };
}

