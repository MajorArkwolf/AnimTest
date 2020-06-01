#pragma once
#include "Model/Models/KeyFrame.hpp"
#include <vector>

namespace Model {
    class Animation {
      public:
        double length = 0;
        std::vector<KeyFrame> keyFrames = {};
        Animation(double time, const std::vector<KeyFrame>& newFrames);
        double getLength() const;
        std::vector<KeyFrame> getKeyFrames();
    };
}

