#pragma once

// Include vulkan.hpp before glfw
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Utility/VulpexMaths.hpp"

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
    VkInstance m_vulkanInstance = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_logicalDevice = VK_NULL_HANDLE;
	// TODO: make this a vector
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;

	#ifdef _DEBUG
		VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	#endif

    // Window resources
    GLFWwindow* m_window = nullptr;

    IVec2 m_winDimensions = {0, 0};

	// Misc variables
	#ifdef _DEBUG
		std::vector<const char*> m_enabledValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		VkDebugUtilsMessageSeverityFlagsEXT m_severitiesToLog = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	#endif

	// Helper functions
	void CreateVulkanInstance(VkApplicationInfo appInfo, std::vector<const char*> vkExtensions, VkInstanceCreateFlags vkFlags);
	void SelectPhysicalDevice();
	void CreateLogicalDevice();

public:
    VulkanApplication();

	void Init(WindowInfo winInfo, VkApplicationInfo appInfo, std::vector<const char*> vkExtensions, VkInstanceCreateFlags vkFlags);

	// Setters
	#ifdef _DEBUG
		// Set this before Init - it will do nothing if set afterwards
		void SetSeveritiesToLog(VkDebugUtilsMessageSeverityFlagsEXT severities) { m_severitiesToLog = severities; }
	#endif

	// Getters
    float GetAspect() const { return m_winDimensions.x / (float)m_winDimensions.y; }

    // Bools
    bool IsRunning() const { return !glfwWindowShouldClose(m_window); };

    ~VulkanApplication();
};