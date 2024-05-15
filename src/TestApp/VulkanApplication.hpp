#pragma once

// Include vulkan.hpp before glfw
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Maths.hpp"

struct WindowInfo
{
	GLFWmonitor* targetMonitor = nullptr;

	const char* title = "Vulkan Application";

    int width = 0;
    int height = 0;
};

class VulkanApplication
{
    // Vulkan resources
    VkInstance vulkanInstance;

    // Window resources
    GLFWwindow* m_window;

    IVec2 m_winDimensions = {0, 0};

    void Update(float delta);
    void Render();

public:
    VulkanApplication();
    
	void Init(WindowInfo winInfo, VkApplicationInfo appInfo, std::vector<const char*> vulkanExtensions, VkInstanceCreateFlags vkFlags);
    void Run();

	// Getters
    float GetAspect() { return m_winDimensions.x / (float)m_winDimensions.y; }

    // Bools
    bool IsRunning();

    ~VulkanApplication();
};