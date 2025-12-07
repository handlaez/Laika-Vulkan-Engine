#include "keyboard_movement_controller.hpp"
#include <iostream>

namespace le {
	void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float timestep, LeActor& actor)
	{
		if (!window) {
			std::cerr << "Window is NULL\n";
		}

		glm::vec3 rotation{ 0 };
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotation.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotation.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotation.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotation.x -= 1.f;

		if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
			actor.transform.rotation += lookSpeed * timestep * glm::normalize(rotation);
		}

		// limiting up/down pitch between +/- 85 degrees
		actor.transform.rotation.x = glm::clamp(actor.transform.rotation.x, -1.5f, 1.5f);
		// variable overflow prevention from spinning
		actor.transform.rotation.y = glm::mod(actor.transform.rotation.y, glm::two_pi<float>());

		float yaw = actor.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackwards) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			actor.transform.translation += moveSpeed * timestep * glm::normalize(moveDir);
		}
	}
}