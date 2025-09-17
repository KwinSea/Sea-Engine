#include "Functions.h"

extern cLightManager* g_pLights;

void editLight(
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
