#pragma once
#include <map>
#include <string>
#include "Model/Models/JointTransform.hpp"
namespace Model {
    class KeyFrame {
      public:
        explicit KeyFrame(double newTimeStamp);
        double timeStamp = 0.0f;
        std::map<std::string, JointTransform> pose = {};
      public:
        KeyFrame(double newTimeStamp, const std::map<std::string, JointTransform>& newPose);
        double getTimeStamp() const;
        std::map<std::string, JointTransform>& getJointKeyFrames();
    };
}

