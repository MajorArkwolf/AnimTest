#pragma once

#include <vector>
#include <assimp/mesh.h>
#include <assimp/matrix4x4.h>

class Bone {
  public:
    aiMatrix4x4 transformation;

    unsigned int parentBoneId;
    bool hasParentBoneId;

    Bone() : hasParentBoneId(false) {}
};

class BoneWeights {
  public:
    aiMatrix4x4 offsetMatrix;
    std::vector<aiVertexWeight> weights;

    unsigned int boneId;
};