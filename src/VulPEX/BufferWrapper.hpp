#pragma once

#include "Utility/VulkanDynamicInclude.hpp"

class BufferWrapper
{
	// Vulkan resources
	vk::Buffer m_buffer;
	vk::DeviceMemory m_bufferMemory;

	vk::BufferCreateInfo m_bufferInfo;

	// Other resources
	uint32_t m_elementSize;
	uint32_t m_elementCount;

	// Helpers
	static uint32_t findMemoryType(vk::PhysicalDevice physDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);

public:
	void CreateBuffer(vk::PhysicalDevice physDevice, vk::Device virtualDevice, vk::BufferCreateInfo bufferInfo, vk::MemoryPropertyFlags memoryProperties);
	void FillBuffer(vk::Device device, void* data, uint32_t elementSize, uint32_t elementCount);

	// Getters
	vk::Buffer GetBuffer() const { return m_buffer; };

	// Cleanup
	void DestroyBuffer(vk::Device device);
};