#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class SurfaceWrapper
{
	VkSurfaceKHR m_displaySurface = VK_NULL_HANDLE;

public:
	void CreateDisplaySurface(VkInstance instance, GLFWwindow* window);

	// Getters
	VkSurfaceKHR GetSurface() const { return m_displaySurface; };

	// Cleanup
	void DestroySurface(VkInstance instance);
};