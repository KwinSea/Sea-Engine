// Credit to Victor Gordan for insperation
// https://youtu.be/86_pQCKOIPk?si=DE5egKig4sAK8XnP
#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include "GlobalOpenGL.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include  "cShaderManager/cShaderManager.h"

extern double deltaTime;
extern bool usingGui;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    int viewWidth;
    int viewHeight;

    bool firstClick = true;

    float speed = 15.0f;
    float sensitivity = 625.5f;

    Camera(int viewWidth, int viewHeight, glm::vec3 position);

    bool isShiftDown(GLFWwindow* window);
    bool isControlDown(GLFWwindow* window);
    bool isAltDown(GLFWwindow* window);
    bool areAllModifiersUp(GLFWwindow* window);

    void Matrix(float FOVdeg, float zNear, float zFar, cShaderManager& shader, const char* uniform);
    void InputHandler(GLFWwindow* window);
};
