#ifndef KEYBOARD_MOVEMENT_CONTROLLER_HPP
#define KEYBOARD_MOVEMENT_CONTROLLER_HPP

#include "le_actor.hpp"
#include "le_window.hpp"

namespace le {
	class KeyboardMovementController {
	public:
		struct KeyMappings {
			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackwards = GLFW_KEY_S;
			int moveUp = GLFW_KEY_SPACE;
			int moveDown = GLFW_KEY_LEFT_CONTROL;
			int lookLeft = GLFW_KEY_LEFT; //TODO: mouse camera control
			int lookRight = GLFW_KEY_RIGHT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
			int toggleHitboxes = GLFW_KEY_H;
		};

		void moveInPlaneXZ(GLFWwindow* window, float timestep, LeActor& actor);
		bool getToggleHitboxPressed(GLFWwindow* window) const;

		KeyMappings keys{};
		float moveSpeed{ 3.f };
		float lookSpeed{ 1.5f };
	};
}

#endif