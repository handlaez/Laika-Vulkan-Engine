#include "keyboard_movement_controller.hpp"

#include <glm/gtx/norm.hpp>
#include <iostream>

namespace le {
    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float deltatime, LeActor& actor, bool viewportHovered)
    {
        float speed = moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 4.0f;
        }

        glm::vec3 forward = actor.transform.rotation * glm::vec3(0, 0, 1);
        glm::vec3 right = actor.transform.rotation * glm::vec3(1, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::vec3 moveDir{ 0.f };
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)     moveDir += forward;
        if (glfwGetKey(window, keys.moveBackwards) == GLFW_PRESS)   moveDir -= forward;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)       moveDir += right;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)        moveDir -= right;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)          moveDir += up;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)        moveDir -= up;

        if (glm::length2(moveDir) > 0.0001f) {
            actor.transform.translation += speed * deltatime * glm::normalize(moveDir);
        }

        if (!viewportHovered)
        {
            firstClick = true;
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_NORMAL
            );
            return;
        }

        // rot
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            float centerX = (float)width / 2.0f;
            float centerY = (float)height / 2.0f;

            float deltaX = (float)mouseX - centerX;
            float deltaY = (float)mouseY - centerY;

            if (firstClick || std::abs(deltaX) > width || std::abs(deltaY) > height) {
                glfwSetCursorPos(window, (double)centerX, (double)centerY);
                firstClick = false;
                return;
            }

            float sensitivityMultiplier = mouseSensitivity * 100.0f;
            pitch += (deltaY / height) * sensitivityMultiplier;
            yaw += (deltaX / width) * sensitivityMultiplier;

            pitch = glm::clamp(pitch, -glm::half_pi<float>() + 0.1f, glm::half_pi<float>() - 0.1f);
            actor.transform.rotation = glm::quat(glm::vec3(pitch, yaw, 0.0f));

            glfwSetCursorPos(window, (double)centerX, (double)centerY);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstClick = true;
        }
    }
}