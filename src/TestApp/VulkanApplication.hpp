#pragma once

// Include vulkan.hpp before glfw
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

struct Resolution
{
    int width = 800;
    int height = 600;

    float GetAspect() { return width / (float)height; }
};

class VulkanApplication
{
    // Vulkan resources
    VkInstance vulkanInstance;

    // Window resources
    GLFWwindow* m_window;

    Resolution m_res;

    void Update(float delta);
    void Render();

public:
    VulkanApplication();
    
	void Init();
    void Run();

    // Bools
    bool IsRunning();

    ~VulkanApplication();
};