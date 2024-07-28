#pragma once

#include <unordered_map>
#include <string>

// Include vulkan.hpp before glfw
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Utility/VulPEXMaths.hpp"

#include "WindowWrapper.hpp"
#include "DebugMessengerWrapper.hpp"
#include "SurfaceWrapper.hpp"
#include "PhysicalDeviceWrapper.hpp"
#include "LogicalDeviceWrapper.hpp"
#include "SwapChainWrapper.hpp"

class VulkanApplication
{
    // Vulkan resources
    vk::Instance m_vulkanInstance = nullptr;

	#ifdef _DEBUG
		DebugMessengerWrapper m_debugMessenger;
	#endif

	SurfaceWrapper m_displaySurface;
	PhysicalDeviceWrapper m_physicalDevice;
	LogicalDeviceWrapper m_logicalDevice;

	SwapChainWrapper m_swapChain;
	
	// GLFW resources
	WindowWrapper m_window;

	// Helper functions
	void CreateVulkanInstance(vk::ApplicationInfo appInfo, std::vector<const char *> vkExtensions, vk::InstanceCreateFlags vkFlags);

public:
    VulkanApplication(std::map<int, int> windowHints)
		: m_window(windowHints) {};

	void Init(WindowInfo winInfo, vk::ApplicationInfo appInfo, std::vector<const char*> vkExtensions, vk::InstanceCreateFlags vkFlags);

	// Getters
	#ifdef _DEBUG
		DebugMessengerWrapper GetDebugMessenger() const { return m_debugMessenger; };
	#endif

	PhysicalDeviceWrapper GetPhysicalDevice() const { return m_physicalDevice; };
	LogicalDeviceWrapper GetLogicalDevice() const { return m_logicalDevice; };
	SurfaceWrapper GetSurface() const { return m_displaySurface; };

    // Bools
    bool IsRunning() const { return m_window.IsWindowRunning(); };

    ~VulkanApplication();
};