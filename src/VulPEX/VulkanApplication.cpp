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
void VulkanApplication::Init(WindowInfo winInfo, vk::ApplicationInfo appInfo, ShaderInfo shaderInfo, std::vector<const char*> vkExtensions, vk::InstanceCreateFlags vkFlags)
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
	
	// Create a graphics pipeline to run shaders and draw our image
	m_graphicsPipeline.CreateGraphicsPipeline(m_logicalDevice.GetLogicalDevice(), shaderInfo, m_swapChain.GetExtent(), m_swapChain.GetFormat());

	m_swapChain.CreateFramebuffers(m_logicalDevice.GetLogicalDevice(), m_graphicsPipeline.GetRenderPass());

	m_commandBuffer.CreateCommandBuffer(m_logicalDevice.GetLogicalDevice(), m_logicalDevice.GetQueueFamilyIndices().queueFamilies["graphicsQueueFamily"]);

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

void VulkanApplication::RenderFrame()
{
	vk::Result result;

	result = m_logicalDevice.GetLogicalDevice().waitForFences(m_startRender, vk::True, UINT64_MAX);
	if (result == vk::Result::eTimeout)
	{
		throw std::runtime_error("Timed out while waiting for fence \"m_startRender\"");
	}
	m_logicalDevice.GetLogicalDevice().resetFences(m_startRender);

	uint32_t scImageIndex;
	std::tie(result, scImageIndex) = m_logicalDevice.GetLogicalDevice().acquireNextImageKHR(m_swapChain.GetSwapchain(), UINT64_MAX, m_imageAvailable, nullptr);
	if (result == vk::Result::eTimeout)
	{
		throw std::runtime_error("Timed out while acquiring next swapchain image");
	}
	
	m_commandBuffer.RecordToCommandBuffer(m_graphicsPipeline.GetRenderPass(), m_swapChain.GetFramebuffer(scImageIndex),
										  m_swapChain.GetExtent(), m_graphicsPipeline.GetPipeline());
	
	vk::PipelineStageFlags pipelineStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	vk::CommandBuffer commandBuffer = m_commandBuffer.GetCommandBuffer();
	vk::SubmitInfo submitInfo(
		1,										//waitSemaphoreCount
		&m_imageAvailable,						//pWaitSemaphores
		&pipelineStage,							//pWaitDstStageMask
		1,										//commandBufferCount
		&commandBuffer,							//pCommandBuffers
		1,										//signalSemaphoreCount
		&m_renderFinished						//pSignalSemaphores
	);

	m_logicalDevice.GetQueue("graphicsQueue").submit(submitInfo, m_startRender);

	vk::SwapchainKHR swapchain = m_swapChain.GetSwapchain();
	vk::PresentInfoKHR presentInfo(
		1,								//waitSemaphoreCount
		&m_renderFinished,				//pWaitSemaphores
		1,								//swapchainCount
		&swapchain,						//pSwapchains
		&scImageIndex,					//pImageIndices
		nullptr							//pResults
	);

	(void) m_logicalDevice.GetQueue("surfaceQueue").presentKHR(presentInfo);
}

VulkanApplication::~VulkanApplication()
{
	// TODO: Find somewhere better to destroy these
	if (m_imageAvailable != nullptr) { m_logicalDevice.GetLogicalDevice().destroySemaphore(m_imageAvailable); }
	if (m_renderFinished != nullptr) { m_logicalDevice.GetLogicalDevice().destroySemaphore(m_renderFinished); }
	if (m_startRender != nullptr) { m_logicalDevice.GetLogicalDevice().destroyFence(m_startRender); }

	m_commandBuffer.DestroyCommandBuffer(m_logicalDevice.GetLogicalDevice());

	m_graphicsPipeline.DestroyPipeline(m_logicalDevice.GetLogicalDevice());

	m_swapChain.DestroySwapChain(m_logicalDevice.GetLogicalDevice());

	m_logicalDevice.DestroyLogicalDevice();

	m_displaySurface.DestroySurface(m_vulkanInstance);

	#ifdef _DEBUG
		m_debugMessenger.DestroyDebugMessenger(m_vulkanInstance);
	#endif

	if (m_vulkanInstance != nullptr) { m_vulkanInstance.destroy(); }

	m_window.DestroyWindow();

    glfwTerminate();
}