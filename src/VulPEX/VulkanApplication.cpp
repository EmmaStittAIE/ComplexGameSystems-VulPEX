#include "VulkanApplication.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include <vector>
#include <map>
#include <set>

#include <Logger.hpp>

#include "Utility/VulPEXUtils.hpp"

// Private Methods

void VulkanApplication::CreateVulkanInstance(vk::ApplicationInfo appInfo, std::vector<const char *> vkExtensions, vk::InstanceCreateFlags vkFlags)
{
	// Get Extension Info
	std::vector<const char*> requiredExtensions = VkUtils::GetRequiredExtensions();

	// Add reqired extensions to vkExtensions
	vkExtensions.reserve(vkExtensions.size() + requiredExtensions.size());
	vkExtensions.insert(vkExtensions.end(), requiredExtensions.begin(), requiredExtensions.end());

	if (!VkUtils::AreInstanceExtensionsSupported(vkExtensions))
	{
		throw std::runtime_error("One or more of the extensions specified are not supported by the target system"); 
	}

	// Validation layers
	uint32_t enabledLayerCount = 0;
	const char* const* enabledLayerNames = nullptr;
	const vk::DebugUtilsMessengerCreateInfoEXT* nextPointer = nullptr;

	#ifdef _DEBUG
		std::vector<const char *> validationLayers = m_debugMessenger.GetValidationLayers();

		enabledLayerCount = validationLayers.size();
		enabledLayerNames = validationLayers.data();

		nextPointer = m_debugMessenger.GetDebugMessengerInfoPointer();
	#endif

	// Configure Instance Info
	vk::InstanceCreateInfo instanceInfo(
		vkFlags,								//flags
		&appInfo,								//pApplicationInfo
		enabledLayerCount,						//enabledLayerCount
		enabledLayerNames,						//ppEnabledLayerNames
		(uint32_t)vkExtensions.size(),			//enabledExtensionCount
		vkExtensions.data(),					//ppEnabledExtensionNames
		nextPointer								//pNext
	);

	// Vulkan.hpp automatically throws exceptions, so we don't have to do that manually anymore
	// This almost feels too simple...
	m_vulkanInstance = vk::createInstance(instanceInfo);
}

// Public Methods
void VulkanApplication::Init(WindowInfo winInfo, vk::ApplicationInfo appInfo, std::vector<const char*> vkExtensions,
							 vk::InstanceCreateFlags vkFlags)
{
	VULKAN_HPP_DEFAULT_DISPATCHER.init();

    // Create window
    m_window.CreateWindow(winInfo);

    // --Init Vulkan--
	// Set up our debug messenger. We need to initialise Vulkan before we create the messenger,
	// but we need info from here to initialise Vulkan in debug mode
	#ifdef _DEBUG
		m_debugMessenger.SetUpDebugCallback();
	#endif

	// Initialise vulkan
	CreateVulkanInstance(appInfo, vkExtensions, vkFlags);
	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vulkanInstance);

	// Initialise debug messenger
	#ifdef _DEBUG
		m_debugMessenger.LinkDebugCallback(m_vulkanInstance);
	#endif

	// Create the GLFW surface we'll be using in our swapchain
	m_displaySurface.CreateDisplaySurface(m_vulkanInstance, m_window.GetWindow());

	// Find and select a GPU to render with
	m_physicalDevice.SelectDevice(m_vulkanInstance, m_displaySurface.GetSurface());

	// Create a logical device to interface with our physical device
	#ifdef _DEBUG
	m_logicalDevice.CreateLogicalDevice(m_physicalDevice.GetPhysicalDevice(), m_displaySurface.GetSurface(), m_physicalDevice.GetDeviceExtensions(),
										m_debugMessenger.GetValidationLayers());
	#else
	m_logicalDevice.CreateLogicalDevice(m_physicalDevice.GetPhysicalDevice(), m_displaySurface.GetSurface(), m_physicalDevice.GetDeviceExtensions());
	#endif

	VULKAN_HPP_DEFAULT_DISPATCHER.init(m_logicalDevice.GetLogicalDevice());

	// Create a swapchain to present images to the screen with
	m_swapChain.CreateSwapChain(m_logicalDevice.GetLogicalDevice(), m_displaySurface.GetSurface(), m_window.GetWindow(),
								m_physicalDevice.GetSwapChainSupportInfo(), m_logicalDevice.GetQueueFamilyIndices());
}

void VulkanApplication::GraphicsPipelineSetup(ShaderInfo shaderInfo, uint32_t sizeOfVertex, std::pair<vk::Format, uint32_t>* vertexVarsInfo,
											  size_t vertexVarsInfoCount, std::vector<DataStructures::Vertex> verts)
{
	// Create a graphics pipeline to run shaders and draw our image
	m_graphicsPipeline.CreateGraphicsPipeline(m_logicalDevice.GetLogicalDevice(), shaderInfo, m_swapChain.GetExtent(), m_swapChain.GetFormat(),
											  sizeOfVertex, vertexVarsInfo, vertexVarsInfoCount);

	// Create framebuffers to display our image
	m_swapChain.CreateFramebuffers(m_logicalDevice.GetLogicalDevice(), m_graphicsPipeline.GetRenderPass());

	// Create vertex buffers to so we can send our vertex data to the GPU
	// Staging buffer
	// TODO: This needs to be made on the fly when loading data, eventually
	uint32_t qfIndicesArray[] = { m_logicalDevice.GetQueueFamilyIndices().queueFamilies.at("graphicsQueueFamily"),
								  m_logicalDevice.GetQueueFamilyIndices().queueFamilies.at("transferQueueFamily") };
	vk::BufferCreateInfo vertexBufferInfo(
		{},											//flags
		sizeOfVertex * verts.size(),				//size
		vk::BufferUsageFlagBits::eTransferSrc,		//usage
		vk::SharingMode::eConcurrent,				//sharingMode
		2,											//queueFamilyIndexCount
		qfIndicesArray								//pQueueFamilyIndices
	);
	// TODO: Those MemoryProperty bits should probably be customisable
	m_vertexStagingBuffer.CreateBuffer(m_physicalDevice.GetPhysicalDevice(), m_logicalDevice.GetLogicalDevice(), vertexBufferInfo,
									   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	//Device buffer
	vertexBufferInfo.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	m_vertexDeviceBuffer.CreateBuffer(m_physicalDevice.GetPhysicalDevice(), m_logicalDevice.GetLogicalDevice(), vertexBufferInfo,
									  vk::MemoryPropertyFlagBits::eDeviceLocal);

	m_graphicsCommandPool.CreateCommandPool(m_logicalDevice.GetLogicalDevice(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
											m_logicalDevice.GetQueueFamilyIndices().queueFamilies.at("graphicsQueueFamily"));
	m_renderCommandBufferIndex = m_graphicsCommandPool.CreateCommandBuffers(m_logicalDevice.GetLogicalDevice(), vk::CommandBufferLevel::ePrimary, 1)[0];

	m_transientTransferCommandPool.CreateCommandPool(m_logicalDevice.GetLogicalDevice(), vk::CommandPoolCreateFlagBits::eTransient,
													 m_logicalDevice.GetQueueFamilyIndices().queueFamilies.at("transferQueueFamily"));

	// TODO: Find somewhere better to initialise these
	// This has no functionality as of yet, but we have to specify it in case a future version of Vulkan defines some
	vk::SemaphoreCreateInfo semaphoreInfo;
	vk::FenceCreateInfo fenceInfo(
		vk::FenceCreateFlagBits::eSignaled	//flags
	);

	m_imageAvailable = m_logicalDevice.GetLogicalDevice().createSemaphore(semaphoreInfo);
	m_renderFinished = m_logicalDevice.GetLogicalDevice().createSemaphore(semaphoreInfo);
	m_startRender = m_logicalDevice.GetLogicalDevice().createFence(fenceInfo);
}

void VulkanApplication::RenderFrame(uint32_t sizeOfVertex, std::vector<DataStructures::Vertex> verts)
{
	vk::Result result;

	result = m_logicalDevice.GetLogicalDevice().waitForFences(m_startRender, vk::True, UINT64_MAX);
	if (result == vk::Result::eTimeout)
	{
		throw std::runtime_error("Timed out while waiting for fence \"m_startRender\"");
	}
	m_graphicsCommandPool.ResetCommandPool(m_logicalDevice.GetLogicalDevice());
	m_logicalDevice.GetLogicalDevice().resetFences(m_startRender);

	uint32_t scImageIndex;
	std::tie(result, scImageIndex) = m_logicalDevice.GetLogicalDevice().acquireNextImageKHR(m_swapChain.GetSwapchain(), UINT64_MAX, m_imageAvailable, nullptr);
	if (result == vk::Result::eTimeout)
	{
		throw std::runtime_error("Timed out while acquiring next swapchain image");
	}
	

	m_vertexStagingBuffer.FillBuffer(m_logicalDevice.GetLogicalDevice(), verts.data(), sizeOfVertex, verts.size());

	m_vertexStagingBuffer.CopyBuffer(m_logicalDevice.GetLogicalDevice(), m_logicalDevice.GetQueue("transferQueue"), &m_transientTransferCommandPool,
									 m_vertexDeviceBuffer.GetBuffer());

	// Graphics buffer recording start
	vk::Extent2D scExtent = m_swapChain.GetExtent();

	m_graphicsCommandPool.BeginRecordingToBuffer(m_renderCommandBufferIndex);

	vk::ClearValue clearValue({ 0.0f, 0.0f, 0.0f, 1.0f });
	vk::RenderPassBeginInfo rpBeginInfo(
		m_graphicsPipeline.GetRenderPass(),				//renderPass
		m_swapChain.GetFramebuffer(scImageIndex),			//framebuffer
		{ {0, 0}, scExtent },	//renderArea
		1,						//clearValueCount
		&clearValue				//pClearValues
	);

	// Render pass start
	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).beginRenderPass(rpBeginInfo, vk::SubpassContents::eInline);

	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).bindPipeline(vk::PipelineBindPoint::eGraphics, m_graphicsPipeline.GetPipeline());

	vk::Buffer vertexBuffers[] = { m_vertexDeviceBuffer.GetBuffer() };
	vk::DeviceSize offsets[] = { 0 };
	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).bindVertexBuffers(0, 1, vertexBuffers, offsets);

	vk::Viewport viewport(
		0,					//x
		0,					//y
		scExtent.width,		//width
		scExtent.height,	//height
		0,					//minDepth
		1					//maxDepth
	);
	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).setViewport(0, viewport);

	vk::Rect2D scissorRect(
		{0, 0},		//offset
		scExtent	//extent
	);
	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).setScissor(0, scissorRect);

	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).draw(
		verts.size(),	//vertexCount
		1,				//instanceCount
		0,				//firstVertex
		0				//firstInstance
	);

	// Render pass finish
	m_graphicsCommandPool.GetCommandBuffer(m_renderCommandBufferIndex).endRenderPass();

	// Graphics buffer recording finish
	m_graphicsCommandPool.EndRecordingToBuffer(m_renderCommandBufferIndex);
	
	vk::PipelineStageFlags pipelineStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	std::vector<vk::CommandBuffer> graphicsCommandBuffers = m_graphicsCommandPool.GetCommandBuffers();
	vk::SubmitInfo submitInfo(
		1,									//waitSemaphoreCount
		&m_imageAvailable,					//pWaitSemaphores
		&pipelineStage,						//pWaitDstStageMask
		graphicsCommandBuffers.size(),		//commandBufferCount
		graphicsCommandBuffers.data(),		//pCommandBuffers
		1,									//signalSemaphoreCount
		&m_renderFinished					//pSignalSemaphores
	);

	m_logicalDevice.GetQueue("graphicsQueue").submit(submitInfo, m_startRender);

	vk::SwapchainKHR swapchain = m_swapChain.GetSwapchain();
	vk::PresentInfoKHR presentInfo(
		1,						//waitSemaphoreCount
		&m_renderFinished,		//pWaitSemaphores
		1,						//swapchainCount
		&swapchain,				//pSwapchains
		&scImageIndex,			//pImageIndices
		nullptr					//pResults
	);

	(void) m_logicalDevice.GetQueue("surfaceQueue").presentKHR(presentInfo);
}

VulkanApplication::~VulkanApplication()
{
	vk::Device logicalDevice = m_logicalDevice.GetLogicalDevice();

	// TODO: Find somewhere better to destroy these
	if (m_imageAvailable != nullptr) { logicalDevice.destroySemaphore(m_imageAvailable); }
	if (m_renderFinished != nullptr) { logicalDevice.destroySemaphore(m_renderFinished); }
	if (m_startRender != nullptr) { logicalDevice.destroyFence(m_startRender); }

	m_transientTransferCommandPool.DestroyCommandPool(logicalDevice);
	m_graphicsCommandPool.DestroyCommandPool(logicalDevice);

	m_vertexDeviceBuffer.DestroyBuffer(logicalDevice);
	m_vertexStagingBuffer.DestroyBuffer(logicalDevice);

	m_graphicsPipeline.DestroyPipeline(logicalDevice);

	m_swapChain.DestroySwapChain(logicalDevice);

	m_logicalDevice.DestroyLogicalDevice();

	m_displaySurface.DestroySurface(m_vulkanInstance);

	#ifdef _DEBUG
		m_debugMessenger.DestroyDebugMessenger(m_vulkanInstance);
	#endif

	if (m_vulkanInstance != nullptr) { m_vulkanInstance.destroy(); }

	m_window.DestroyWindow();

    glfwTerminate();
}