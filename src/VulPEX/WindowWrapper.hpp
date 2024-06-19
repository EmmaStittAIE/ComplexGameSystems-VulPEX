#pragma once

#include <map>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Utility/VulPEXMaths.hpp"

struct WindowInfo
{
	GLFWmonitor* targetMonitor = nullptr;

	const char* title = "Vulkan Application";

    int width = 0;
    int height = 0;
};

class WindowWrapper
{
	GLFWwindow* m_window = nullptr;

	IVec2 m_winDimensions = {0, 0};

public:
	WindowWrapper(std::map<int, int> windowHints);

	void CreateWindow(WindowInfo winInfo);

	// Getters
	GLFWwindow* GetWindow() const { return m_window; };
	
	IVec2 GetDimensions() const { return m_winDimensions; }
	float GetAspectRatio() const { return m_winDimensions.x / (float)m_winDimensions.y; }

	// Bools
	bool IsWindowRunning() const { return !glfwWindowShouldClose(m_window); };

	// Cleanup
	void DestroyWindow();
};