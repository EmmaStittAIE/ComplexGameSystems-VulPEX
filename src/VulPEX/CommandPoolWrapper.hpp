#pragma once

#include "Utility/VulkanDynamicInclude.hpp"

class CommandPoolWrapper
{
	// Vulkan Resources
	vk::CommandPool m_commandPool = nullptr;
	std::vector<vk::CommandBuffer> m_commandBuffers;

public:
	void CreateCommandPool(vk::Device device, vk::CommandPoolCreateFlagBits bufferType, uint32_t queueFamilyIndex);
	std::vector<uint32_t>  CreateCommandBuffers(vk::Device device, vk::CommandBufferLevel bufferLevel, uint32_t numBuffers);

	void BeginRecordingToBuffer(uint32_t bufferIndex, vk::CommandBufferUsageFlagBits usageFlags = {},
								vk::CommandBufferInheritanceInfo secondaryInheritanceInfo = {});
	void EndRecordingToBuffer(uint32_t bufferIndex);
	
	void ResetCommandPool(vk::Device device);
	void EmptyCommandPool(vk::Device device);

	// Getters
	vk::CommandPool GetCommandPool() const { return m_commandPool; };

	vk::CommandBuffer GetCommandBuffer(uint32_t bufferIndex) const { return m_commandBuffers[bufferIndex]; };
	std::vector<vk::CommandBuffer> GetCommandBuffers() const { return m_commandBuffers; }

	// Cleanup
	void DestroyCommandPool(vk::Device device);
};