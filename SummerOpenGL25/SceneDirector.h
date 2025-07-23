#pragma once
#include <vector>

#include "cLightManager.h"
#include "cMeshObject.h"

class SceneDirector {
};

void AddMeshObject (
    std::string meshFileName,
    std::string uniqueName = "",
    glm::vec3 position = glm::vec3(0.0f),
    glm::vec3 orientation = glm::vec3(0.0f),
    float scale = 1.0f,
    glm::vec3 colourRGB = glm::vec3(1.0f),
    glm::vec3 specularHighLightRGB = glm::vec3(1.0f),
    float specularPower = 1.0f,

    bool bOverrideVertexModelColour = false,
    bool bIsWireframe = false,
    bool bIsVisible = true
);