#pragma once
#include "GlobalOpenGL.h"
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include <sstream>

class cLight {
public:
    cLight();
    glm::vec4 position; // xyz, ignoring w (4th parameter)
    glm::vec4 diffuse;
    glm::vec4 specular; // rgb = highlight colour, w = power
    glm::vec4 atten; // x = constant, y = linear, z = quadratic, w = DistanceCutOff
    glm::vec4 direction; // Spot, directional lights, ignoring w (4th parameter)
    glm::vec4 param1; // x = lightType, y = inner angle, z = outer angle, w = TBD
    // 0 = pointlight
    // 1 = spot light
    // 2 = directional light
    glm::vec4 param2; // x = 0 for off, 1 for on


    GLint position_UL = -1;
    GLint diffuse_UL = -1;
    GLint specular_UL = -1;
    GLint atten_UL = -1;
    GLint direction_UL = -1;
    GLint param1_UL = -1;
    GLint param2_UL = -1;
};


class cLightManager {
public:
    //cLightManager();

    void GetUniformLocations(GLuint shaderProgram);
    void UpdateShaderUniforms(GLuint shaderProgram);

    static const int NUMBEROFLIGHTS = 20;
    cLight theLights[NUMBEROFLIGHTS];
};
