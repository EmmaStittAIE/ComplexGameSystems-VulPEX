#include "SurfaceWrapper.hpp"

void SurfaceWrapper::CreateDisplaySurface(vk::Instance instance, GLFWwindow* window)
{
	// Unfortunately, have to do this in the C way because of glfw
	// vk::SurfaceKHR should be the same size as VkSurfaceKHR, according to the internet, so I hope and pray this doesn't break
	VkResult createSurfaceResult = glfwCreateWindowSurface(instance, window, nullptr, (VkSurfaceKHR*)&m_displaySurface);
	if (createSurfaceResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of display surface failed with error code: " + std::to_string(createSurfaceResult));
	}
}

void SurfaceWrapper::DestroySurface(vk::Instance instance)
{
	if (m_displaySurface != nullptr) { instance.destroySurfaceKHR(m_displaySurface); }
}
