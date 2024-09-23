#pragma once

#include <unordered_map>
#include <string>

// Include vulkan.hpp before glfw
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "Utility/VulPEXMaths.hpp"
#include "Modules/DataStructures.hpp"

#include "WindowWrapper.hpp"
#include "DebugMessengerWrapper.hpp"
#include "SurfaceWrapper.hpp"
#include "PhysicalDeviceWrapper.hpp"
#include "LogicalDeviceWrapper.hpp"
#include "SwapChainWrapper.hpp"
#include "GraphicsPipelineWrapper.hpp"
#include "CommandBufferWrapper.hpp"

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

	GraphicsPipelineWrapper m_graphicsPipeline;

	CommandBufferWrapper m_commandBuffer;
	
	// TODO: Find somewhere better to put these
	vk::Semaphore m_imageAvailable = nullptr;
	vk::Semaphore m_renderFinished = nullptr;
	vk::Fence m_startRender = nullptr;

	// GLFW resources
	WindowWrapper m_window;

	// Helper functions
	void CreateVulkanInstance(vk::ApplicationInfo appInfo, std::vector<const char *> vkExtensions, vk::InstanceCreateFlags vkFlags);

public:
    VulkanApplication(std::map<int, int> windowHints)
		: m_window(windowHints) {};

	template <typename VertType = DataStructures::Vertex>
	void Init(WindowInfo winInfo, vk::ApplicationInfo appInfo, ShaderInfo shaderInfo, std::vector<const char*> vkExtensions, vk::InstanceCreateFlags vkFlags);
	void RenderFrame();
	void SynchroniseBeforeQuit() const { m_logicalDevice.GetLogicalDevice().waitIdle(); };

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