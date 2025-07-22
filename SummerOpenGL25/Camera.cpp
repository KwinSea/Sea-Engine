#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

Camera::Camera(int viewWidth, int viewHeight, glm::vec3 position) {
    Camera::viewWidth = viewWidth;
    Camera::viewHeight = viewHeight;
    Position = position;
}

void Camera::Matrix(float FOVdeg, float zNear, float zFar, cShaderManager& shader, const char* uniform) {

    glm::mat4 matView = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4  matProj = glm::perspective(0.6f, viewWidth / (float)viewHeight, 0.1f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader.useShaderProgram(0), uniform), 1, GL_FALSE, glm::value_ptr(matProj * matView));
}


