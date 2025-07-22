// Credit to Victor Gordan for operation
// https://youtu.be/86_pQCKOIPk?si=DE5egKig4sAK8XnP
#pragma once

#include "GlobalOpenGL.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include  "cShaderManager/cShaderManager.h"

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    int viewWidth;
    int viewHeight;

    float speed = 0.15f;
    float sensitivity = 100.0f;

    Camera(int viewWidth, int viewHeight, glm::vec3 position);

    void Matrix(float FOVdeg, float zNear, float zFar, cShaderManager& shader, const char* uniform);
    void InputManger(GLFWwindow* window);
};
