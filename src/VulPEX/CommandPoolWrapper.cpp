#include "CommandPoolWrapper.hpp"

void CommandPoolWrapper::CreateCommandPool(vk::Device device, vk::CommandPoolCreateFlagBits bufferType, uint32_t queueFamilyIndex)
{
	vk::CommandPoolCreateInfo commandPoolInfo(
		bufferType,			//flags
		queueFamilyIndex	//queueFamilyIndex
	);

	m_commandPool = device.createCommandPool(commandPoolInfo);
}

// Public
std::vector<uint32_t> CommandPoolWrapper::CreateCommandBuffers(vk::Device device, vk::CommandBufferLevel bufferLevel, uint32_t numBuffers)
{
	vk::CommandBufferAllocateInfo commandBufferInfo(
		m_commandPool,	//commandPool
		bufferLevel,	//level
		numBuffers		//commandBufferCount
	);

	std::vector<vk::CommandBuffer> commandBuffers = device.allocateCommandBuffers(commandBufferInfo);
	std::vector<uint32_t> newBufferIndices;
	uint32_t numExistingBuffers = m_commandBuffers.size();
	for (int i = 0; i < numBuffers; i++)
	{
		m_commandBuffers.push_back(commandBuffers[i]);
		newBufferIndices.push_back(numExistingBuffers + i);
	}

	return newBufferIndices;
}

void CommandPoolWrapper::BeginRecordingToBuffer(uint32_t bufferIndex, vk::CommandBufferUsageFlagBits usageFlags,
												vk::CommandBufferInheritanceInfo secondaryInheritanceInfo)
{
	vk::CommandBufferBeginInfo cbBeginInfo(
		usageFlags,					//flags
		&secondaryInheritanceInfo	//pInheritanceInfo
	);

	m_commandBuffers[bufferIndex].begin(cbBeginInfo);
}

void CommandPoolWrapper::EndRecordingToBuffer(uint32_t bufferIndex)
{
	m_commandBuffers[bufferIndex].end();
}

void CommandPoolWrapper::ResetCommandPool(vk::Device device)
{
	device.resetCommandPool(m_commandPool);
}

void CommandPoolWrapper::EmptyCommandPool(vk::Device device)
{
	device.freeCommandBuffers(m_commandPool, m_commandBuffers);
	device.resetCommandPool(m_commandPool);

	m_commandBuffers.clear();
}

void CommandPoolWrapper::DestroyCommandPool(vk::Device device)
{
	// Command buffers are automatically freed when the associated command pool is destroyed. Neat!
	if (m_commandPool != nullptr) { device.destroyCommandPool(m_commandPool); }
}
