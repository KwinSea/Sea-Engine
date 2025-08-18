#include "Camera.h"

Camera::Camera(int viewWidth, int viewHeight, glm::vec3 position) {
    Camera::viewWidth = viewWidth;
    Camera::viewHeight = viewHeight;
    Position = position;
}

void Camera::Matrix(float FOVdeg, float zNear, float zFar, cShaderManager& shader, const char* uniform) {

    glm::mat4 matView = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4  matProj = glm::perspective(FOVdeg, viewWidth / (float)viewHeight, zNear, zFar);

    glUniformMatrix4fv(glGetUniformLocation(shader.getIDFromFriendlyName("shader1"), uniform), 1, GL_FALSE, glm::value_ptr(matProj * matView));
}

// Input Stuff
bool Camera::isShiftDown(GLFWwindow* window) {
    if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
    {
        return true;
    }
    return false;
}

bool Camera::isControlDown(GLFWwindow* window)
{
    if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
    {
        return true;
    }
    return false;
}

bool Camera::isAltDown(GLFWwindow* window)
{
    if ((glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) ||
        (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS))
    {
        return true;
    }
    return false;
}

bool Camera::areAllModifiersUp(GLFWwindow* window)
{
    if (isShiftDown(window))
    {
        return false;
    }

    if (isControlDown(window))
    {
        return false;
    }

    if (isAltDown(window))
    {
        return false;
    }

    // Nothing is down
    return true;
}

void Camera::InputHandler(GLFWwindow* window) {

    if (areAllModifiersUp(window)) {
        // Move the camera
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            this->Position += this->speed * glm::normalize(glm::vec3(this->Orientation))  * static_cast<float>(deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            this->Position += this->speed * glm::normalize(glm::vec3(-this->Orientation))  * static_cast<float>(deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            this->Position += this->speed * -glm::normalize(glm::cross(this->Orientation, this->Up)) * static_cast<float>(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            this->Position += this->speed * glm::normalize(glm::cross(this->Orientation, this->Up)) * static_cast<float>(deltaTime);
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            this->Position += this->speed * glm::normalize(-this->Up) * static_cast<float>(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            this->Position += this->speed * glm::normalize(this->Up) * static_cast<float>(deltaTime);
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hides cursor

        if (firstClick)
        {
            glfwSetCursorPos(window, (viewWidth / 2), (viewHeight / 2));
            firstClick = false;
        }

        double mouseX;
        double mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float rotX = sensitivity * (float)(mouseY - ( viewHeight / 2)) / viewHeight * static_cast<float>(deltaTime);
        float rotY = sensitivity * (float)(mouseX - (viewWidth / 2)) / viewWidth * static_cast<float>(deltaTime);

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







