#include "BufferWrapper.hpp"

// Private
uint32_t BufferWrapper::findMemoryType(vk::PhysicalDevice physDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memoryProperties = physDevice.getMemoryProperties();

	for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		// The first memory type that fits the filter and the selected properties will be accepted
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Could not find suitable memory type");
}

// Public
void BufferWrapper::CreateBuffer(vk::PhysicalDevice physDevice, vk::Device virtualDevice, vk::BufferCreateInfo bufferInfo, vk::MemoryPropertyFlags memoryProperties)
{
	m_bufferInfo = bufferInfo;
	m_buffer = virtualDevice.createBuffer(bufferInfo);

	vk::MemoryRequirements memoryRequirements = virtualDevice.getBufferMemoryRequirements(m_buffer);

	uint32_t memoryType = findMemoryType(physDevice, memoryRequirements.memoryTypeBits, memoryProperties);
	vk::MemoryAllocateInfo allocateInfo(
		memoryRequirements.size,	//allocationSize
		memoryType					//memoryTypeIndex
	);

	m_bufferMemory = virtualDevice.allocateMemory(allocateInfo);

	virtualDevice.bindBufferMemory(m_buffer, m_bufferMemory, 0);

	vk::FenceCreateInfo fenceInfo;
	m_copyDone = virtualDevice.createFence(fenceInfo);
}

void BufferWrapper::FillBuffer(vk::Device device, void* data, uint32_t elementSize, uint32_t elementCount)
{
	m_elementSize = elementSize;
	m_elementCount = elementCount;

	void* dataLocation;
	dataLocation = device.mapMemory(m_bufferMemory, 0, m_bufferInfo.size);
	std::memcpy(dataLocation, data, m_elementSize * m_elementCount);
	device.unmapMemory(m_bufferMemory);
}

void BufferWrapper::CopyBuffer(vk::Device device, vk::Queue transferQueue, CommandPoolWrapper* commandPool, vk::Buffer destination)
{
	uint32_t bufferIndex = commandPool->CreateCommandBuffers(device, vk::CommandBufferLevel::ePrimary, 1)[0];

	commandPool->BeginRecordingToBuffer(bufferIndex, vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	vk::BufferCopy copyRegion(
		0,									//srcOffset
		0,									//dstOffset
		m_elementSize * m_elementCount		//size
	);

	commandPool->GetCommandBuffer(bufferIndex).copyBuffer(m_buffer, destination, copyRegion);

	commandPool->EndRecordingToBuffer(bufferIndex);

	vk::CommandBuffer commandBuffer = commandPool->GetCommandBuffer(bufferIndex);
	vk::SubmitInfo submitInfo(
		0,					//waitSemaphoreCount
		nullptr,			//pWaitSemaphores
		nullptr,			//pWaitDstStageMask
		1,					//commandBufferCount
		&commandBuffer,		//pCommandBuffers
		0,					//signalSemaphoreCount
		nullptr				//pSignalSemaphores
	);

	transferQueue.submit(submitInfo, m_copyDone);
	vk::Result result = device.waitForFences(m_copyDone, vk::True, UINT64_MAX);
	if (result == vk::Result::eTimeout)
	{
		throw std::runtime_error("Timed out while waiting for fence \"m_copyDone\"");
	}
	device.resetFences(m_copyDone);

	device.freeCommandBuffers(commandPool->GetCommandPool(), commandPool->GetCommandBuffer(bufferIndex));
}

void BufferWrapper::DestroyBuffer(vk::Device device)
{
	if (m_copyDone != nullptr) { device.destroyFence(m_copyDone); }

	device.destroyBuffer(m_buffer);

	device.freeMemory(m_bufferMemory);
}
