#ifndef KEYBOARD_MOVEMENT_CONTROLLER_HPP
#define KEYBOARD_MOVEMENT_CONTROLLER_HPP

#include "src/objects/le_actor.hpp"
#include "src/core/le_window.hpp"

namespace le {
    struct KeyMappings {
        int moveForward = GLFW_KEY_W;
        int moveBackwards = GLFW_KEY_S;
        int moveRight = GLFW_KEY_D;
        int moveLeft = GLFW_KEY_A;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_CONTROL;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
    };

    class KeyboardMovementController {
    public:
        void moveInPlaneXZ(GLFWwindow* window, float timestep, LeActor& actor, bool viewportHovered);

        KeyMappings keys{};
        float moveSpeed{ 6.f };
        float mouseSensitivity{ 0.03f };

    private:
        bool mouseCaptured_{ false };
        bool firstClick{ false };
        
        double lastMouseX = 0.0;
        double lastMouseY = 0.0;

        float pitch{ 0.0f };
        float yaw{ 0.0f };
    };
}

#endif