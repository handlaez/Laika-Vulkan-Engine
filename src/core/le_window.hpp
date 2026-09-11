#ifndef LE_WINDOW_HPP
#define LE_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

namespace le {

	class LeWindow {
	public:
		LeWindow(int w, int h, std::string name);
		~LeWindow();

		LeWindow(const LeWindow&) = delete;
		LeWindow& operator=(const LeWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};
}

#endif