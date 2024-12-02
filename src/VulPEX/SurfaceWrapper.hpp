#pragma once

#include "Utility/VulkanDynamicInclude.hpp"
#include <GLFW/glfw3.h>

class SurfaceWrapper
{
	vk::SurfaceKHR m_displaySurface = nullptr;

public:
	void CreateDisplaySurface(vk::Instance instance, GLFWwindow* window);

	// Getters
	vk::SurfaceKHR GetSurface() const { return m_displaySurface; };

	// Cleanup
	void DestroySurface(vk::Instance instance);
};