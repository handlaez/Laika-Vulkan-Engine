#include "keyboard_movement_controller.hpp"

#include <glm/gtx/norm.hpp>
#include <iostream>

namespace le {
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float timestep, LeActor& actor)
	{
		if (!window) {
			std::cerr << "Window is NULL\n";
		}

		glm::vec3 rotInput{ 0.f };

		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotInput.y -= 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)  rotInput.y += 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)    rotInput.x -= 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)  rotInput.x += 1.f;

		if (glm::length2(rotInput) > 0.0001f) {
			rotInput = glm::normalize(rotInput);

			glm::quat qPitch = glm::angleAxis(
				lookSpeed * timestep * rotInput.x,
				glm::vec3(1, 0, 0)
			);

			glm::quat qYaw = glm::angleAxis(
				lookSpeed * timestep * rotInput.y,
				glm::vec3(0, 1, 0)
			);

			actor.transform.rotation = glm::normalize(qYaw * qPitch * actor.transform.rotation);
		}

		glm::vec3 forward = actor.transform.rotation * glm::vec3(0, 0, 1);
		glm::vec3 right = actor.transform.rotation * glm::vec3(1, 0, 0);
		glm::vec3 up = glm::vec3(0, -1, 0);

		glm::vec3 moveDir{ 0.f };

		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forward;
		if (glfwGetKey(window, keys.moveBackwards) == GLFW_PRESS) moveDir -= forward;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += right;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= right;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += up;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= up;

		if (glm::length2(moveDir) > 0.0001f) {
			actor.transform.translation += moveSpeed * timestep * glm::normalize(moveDir);
		}
	}
}