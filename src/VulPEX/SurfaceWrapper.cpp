#include "SurfaceWrapper.hpp"

void SurfaceWrapper::CreateDisplaySurface(VkInstance instance, GLFWwindow* window)
{
	VkResult createSurfaceResult = glfwCreateWindowSurface(instance, window, nullptr, &m_displaySurface);
	if (createSurfaceResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of display surface failed with error code: " + std::to_string(createSurfaceResult));
	}
}

void SurfaceWrapper::DestroySurface(VkInstance instance)
{
	if (m_displaySurface != VK_NULL_HANDLE) { vkDestroySurfaceKHR(instance, m_displaySurface, nullptr); }
}
