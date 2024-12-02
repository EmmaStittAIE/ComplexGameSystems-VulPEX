#include "CommandBufferWrapper.hpp"

// Public
void CommandBufferWrapper::CreateCommandBuffer(vk::Device device, uint32_t queueFamilyIndex)
{
	vk::CommandPoolCreateInfo commandPoolInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,		//flags
		queueFamilyIndex										//queueFamilyIndex
	);

	m_commandPool = device.createCommandPool(commandPoolInfo);

	vk::CommandBufferAllocateInfo commandBufferInfo(
		m_commandPool,						//commandPool
		vk::CommandBufferLevel::ePrimary,	//level
		1									//commandBufferCount
	);

	m_commandBuffer = device.allocateCommandBuffers(commandBufferInfo)[0];
}

void CommandBufferWrapper::RecordToCommandBuffer(vk::RenderPass renderPass, vk::Framebuffer frameBuffer, vk::Buffer vertexBuffer, uint32_t vertexCount,
												 vk::Extent2D scExtent, vk::Pipeline graphicsPipeline)
{
	m_commandBuffer.reset();

	vk::CommandBufferBeginInfo cbBeginInfo(
		{},		//flags
		nullptr	//pInheritanceInfo
	);

	m_commandBuffer.begin(cbBeginInfo);

	vk::ClearValue clearValue({ 0.0f, 0.0f, 0.0f, 1.0f });

	vk::RenderPassBeginInfo rpBeginInfo(
		renderPass,				//renderPass
		frameBuffer,			//framebuffer
		{ {0, 0}, scExtent },	//renderArea
		1,						//clearValueCount
		&clearValue				//pClearValues
	);

	m_commandBuffer.beginRenderPass(rpBeginInfo, vk::SubpassContents::eInline);

	m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

	vk::Buffer vertexBuffers[] = { vertexBuffer };
	vk::DeviceSize offsets[] = { 0 };
	m_commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);

	vk::Viewport viewport(
		0,					//x
		0,					//y
		scExtent.width,		//width
		scExtent.height,	//height
		0,					//minDepth
		1					//maxDepth
	);
	m_commandBuffer.setViewport(0, viewport);

	vk::Rect2D scissorRect(
		{0, 0},		//offset
		scExtent	//extent
	);
	m_commandBuffer.setScissor(0, scissorRect);

	m_commandBuffer.draw(
		vertexCount,	//vertexCount
		1,				//instanceCount
		0,				//firstVertex
		0				//firstInstance
	);

	m_commandBuffer.endRenderPass();

	m_commandBuffer.end();
}

void CommandBufferWrapper::DestroyCommandBuffer(vk::Device device)
{
	// Command buffers are automatically freed when the associated command pool is destroyed. Neat!
	if (m_commandPool != nullptr) { device.destroyCommandPool(m_commandPool); }
}
