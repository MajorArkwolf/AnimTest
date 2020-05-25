#include "AssimpGenerator.hpp"
Controller::AssimpGenerator &Controller::AssimpGenerator::get() {
    static auto instance = AssimpGenerator{};
    return instance;
}
Assimp::Importer &Controller::AssimpGenerator::getImporter() {
    return importer;
}
