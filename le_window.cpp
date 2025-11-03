#include "le_window.hpp"

namespace le {

	LeWindow::LeWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } 
	{
		initWindow();
	}

	LeWindow::~LeWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface.");
		}
	}

	void LeWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto leWindow = reinterpret_cast<LeWindow*>(glfwGetWindowUserPointer(window));
		leWindow->framebufferResized = true;
		leWindow->width = width;
		leWindow->height = height;
	}

	void LeWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

}  // namespace Laika Engine (ofc)