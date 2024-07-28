#include "VulkanApplication.hpp"

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
void VulkanApplication::Init(WindowInfo winInfo, vk::ApplicationInfo appInfo, std::vector<const char*> vkExtensions, vk::InstanceCreateFlags vkFlags)
{
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

	// Create a swapchain to present images to the screen with
	m_swapChain.CreateSwapChain(m_logicalDevice.GetLogicalDevice(), m_displaySurface.GetSurface(), m_window.GetWindow(),
								m_physicalDevice.GetSwapChainSupportInfo(), m_logicalDevice.GetQueueFamilyIndices());
}

VulkanApplication::~VulkanApplication()
{
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