#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

namespace Controller {
    class AssimpGenerator {
      private:
        Assimp::Importer importer;
        AssimpGenerator() = default;

      public:
        static AssimpGenerator& get();
        Assimp::Importer& getImporter();
    };
}

