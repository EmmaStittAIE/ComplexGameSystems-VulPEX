#pragma once

#include "Utility/VulkanDynamicInclude.hpp"

class CommandBufferWrapper
{
	// Vulkan Resources
	vk::CommandPool m_commandPool = nullptr;
	vk::CommandBuffer m_commandBuffer = nullptr;

public:
	void CreateCommandBuffer(vk::Device device, uint32_t queueFamilyIndex);

	void RecordToCommandBuffer(vk::RenderPass renderPass, vk::Framebuffer frameBuffer, vk::Buffer vertexBuffer, uint32_t vertexCount,
							   vk::Extent2D scExtent, vk::Pipeline graphicsPipeline);

	// Getters
	vk::CommandBuffer GetCommandBuffer() const { return m_commandBuffer; };

	// Cleanup
	void DestroyCommandBuffer(vk::Device device);
};