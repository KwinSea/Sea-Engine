// Functions that add or edit aspects of the scene

#include "SceneDirector.h"

extern cLightManager* g_pLights;
extern std::vector<cMeshObject*> g_pMeshesToDraw;

void EditLight(
    unsigned int lightIndex,
    float lightState = 0.0,
    float lightType = 0.0,
    glm::vec4 lightPosition = glm::vec4(0.0, 0.0, 0.0, 0.0),
    glm::vec4 lightDiffuse = glm::vec4(0.0, 0.0, 0.0, 0.0)
) {
    // Light 1
    g_pLights->theLights[lightIndex].param2.x = 1.0f; // turn on
    g_pLights->theLights[lightIndex].param1.x = 0.0f; // light type = point light
    g_pLights->theLights[lightIndex].position = glm::vec4(-10.0f, -10.0f, -10.0f, 0.5f);
    g_pLights->theLights[lightIndex].diffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void AddMeshObject (
    std::string meshFileName,
    std::string uniqueName,
    glm::vec3 position,
    glm::vec3 orientation,
    float scale,
    glm::vec3 colourRGB,
    glm::vec3 specularHighLightRGB,
    float specularPower,

    bool bOverrideVertexModelColour,
    bool bIsWireframe,
    bool bIsVisible
) {
    cMeshObject* pNewObject = new cMeshObject();

    // Mesh name
    pNewObject->meshFileName = meshFileName;

    // Position
    pNewObject->position.x = position.x;
    pNewObject->position.y = position.y;
    pNewObject->position.z = position.z;

    // Orientation
    pNewObject->orientation.x = orientation.x;
    pNewObject->orientation.y = orientation.y;
    pNewObject->orientation.z = orientation.z;

    // Scale
    pNewObject->scale = scale;

    // Color
    pNewObject->colourRGB.r = colourRGB.r;
    pNewObject->colourRGB.g = colourRGB.g;
    pNewObject->colourRGB.b = colourRGB.b;

    // Specular highlight
    pNewObject->specularHighLightRGB.r = specularHighLightRGB.r;
    pNewObject->specularHighLightRGB.g = specularHighLightRGB.g;
    pNewObject->specularHighLightRGB.b = specularHighLightRGB.b;

    // Specular power
    pNewObject->specularPower = specularPower;

    // Attributes
    pNewObject->bOverrideVertexModelColour = bOverrideVertexModelColour; // Override color
    pNewObject->bIsVisible = bIsVisible; // is Visible
    pNewObject->bIsWireframe = bIsWireframe; // is Wireframe

    ::g_pMeshesToDraw.push_back(pNewObject);
}