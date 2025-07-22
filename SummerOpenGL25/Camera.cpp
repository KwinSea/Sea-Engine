#include "Camera.h"

Camera::Camera(int viewWidth, int viewHeight, glm::vec3 position) {
    Camera::viewWidth = viewWidth;
    Camera::viewHeight = viewHeight;
    Position = position;
}

void Camera::Matrix(float FOVdeg, float zNear, float zFar, cShaderManager& shader, const char* uniform) {

    glm::mat4 matView = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4  matProj = glm::perspective(0.6f, viewWidth / (float)viewHeight, 0.1f, 1000.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader.getIDFromFriendlyName("shader1"), uniform), 1, GL_FALSE, glm::value_ptr(matProj * matView));
}

void Camera::InputHandler(GLFWwindow* window) {

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hides cursor

        if (firstClick)
        {
            glfwSetCursorPos(window, (viewWidth / 2), (viewHeight / 2));
            firstClick = false;
        }

        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - ( viewHeight / 2)) / viewHeight;
        float rotY = sensitivity * (float)(mouseX - (viewWidth / 2)) / viewWidth;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            Orientation = newOrientation;
        }

        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

        glfwSetCursorPos(window, (viewWidth / 2), (viewHeight / 2));
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Shows cursor
        firstClick = true;
    }
}




