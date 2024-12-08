#pragma once

#include <unordered_map>
#include <string>
#include <array>

// Include vulkan.hpp before glfw
#include "Utility/VulkanDynamicInclude.hpp"
#include <GLFW/glfw3.h>

#include "Utility/VulPEXMaths.hpp"

#include "WindowWrapper.hpp"
#include "DebugMessengerWrapper.hpp"
#include "SurfaceWrapper.hpp"
#include "PhysicalDeviceWrapper.hpp"
#include "LogicalDeviceWrapper.hpp"
#include "SwapChainWrapper.hpp"
#include "GraphicsPipelineWrapper.hpp"
#include "BufferWrapper.hpp"
#include "CommandPoolWrapper.hpp"

#include "Modules/DataStructures/DefaultVertex.hpp"

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

	BufferWrapper m_vertexStagingBuffer;
	BufferWrapper m_vertexDeviceBuffer;
	BufferWrapper m_indexStagingBuffer;
	BufferWrapper m_indexDeviceBuffer;

	CommandPoolWrapper m_graphicsCommandPool;
	uint32_t m_renderCommandBufferIndex;
	CommandPoolWrapper m_transientTransferCommandPool;
	
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

	void Init(WindowInfo winInfo, vk::ApplicationInfo appInfo, std::vector<const char*> vkExtensions, vk::InstanceCreateFlags vkFlags);

	void GraphicsPipelineSetup(ShaderInfo shaderInfo, uint32_t sizeOfVertex, std::pair<vk::Format, uint32_t>* vertexVarsInfo,
							   size_t vertexVarsInfoCount, std::vector<DataStructures::Vertex> verts, std::vector<uint32_t> indices);

	void RenderFrame(uint32_t sizeOfVertex, std::vector<DataStructures::Vertex> verts, std::vector<uint32_t> indices);
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
