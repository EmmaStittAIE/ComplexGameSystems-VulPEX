#include "VulkanApplication.hpp"

#include <vector>
#include <map>
#include <set>

#include <Logger.hpp>

#include "Utility/VulkanExtensionProxies.hpp"
#include "Utility/VulPEXUtils.hpp"

// Callbacks
// Alright, lots to unpack here
// VKAPI_ATTR is a macro placed before the return type of a function, and allows Vulkan to call the function properly using C++11 and GCC/Clang-style compilers
// VkBool32 is just a bool typedef, since Vulkan is based in C, and C doesn't have an official bool type
// VKAPI_CALL is a macro placed after the return type of a function, and allows Vulkan to call the function properly using MSVC-style compilers
#ifdef _DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch(messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				Logger::Log({ pCallbackData->pMessage }, LogType::None);
				break;
			
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				Logger::Log({ pCallbackData->pMessage }, LogType::Info);
				break;
			
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				Logger::Log({ pCallbackData->pMessage }, LogType::Warning);
				break;
			
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				Logger::Log({ pCallbackData->pMessage }, LogType::Error);
				break;

			default:
				throw std::runtime_error("Debug message severity \"" + std::to_string(messageSeverity) + "\" could not be handled");
		}

		return VK_FALSE;
	}
#endif

// Private Methods

void VulkanApplication::CreateVulkanInstance(VkApplicationInfo appInfo, std::vector<const char *> vkExtensions, VkInstanceCreateFlags vkFlags)
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
	const char* const* enabledLayerNames = NULL;
	const VkDebugUtilsMessengerCreateInfoEXT* nextPointer = NULL;

	#ifdef _DEBUG
		if (!VkUtils::AreValidationLayersSupported(m_enabledValidationLayers))
		{
			throw std::runtime_error("One or more of the validation layers specified are not supported by the target system"); 
		}

		enabledLayerCount = m_enabledValidationLayers.size();
		enabledLayerNames = m_enabledValidationLayers.data();

		// Only calls to warning and error will be passed through
		// This needs to be set up now so we can show debug messages for creation and destruction of an instance, even though our debug messenger will be gone
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,											//sType
			NULL,																								//pNext
			0,																									//flags
			m_severitiesToLog,																					//messageSeverity
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,													//messageType
			DebugMessageCallback,																				//pfnUserCallback
			nullptr																								//pUserData
		};

		nextPointer = &debugMessengerInfo;
	#endif

	// Configure Instance Info
	VkInstanceCreateInfo instanceInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	//sType
		nextPointer,							//pNext
		vkFlags,								//flags
		&appInfo,								//pApplicationInfo
		enabledLayerCount,						//enabledLayerCount
		enabledLayerNames,						//ppEnabledLayerNames
		(uint32_t)vkExtensions.size(),			//enabledExtensionCount
		vkExtensions.data()						//ppEnabledExtensionNames
	};

	// Create info, custom memory allocator callback, pointer to instance
	VkResult createInstanceResult = vkCreateInstance(&instanceInfo, nullptr, &m_vulkanInstance);
	if (createInstanceResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of Vulkan instance failed with error code: " + std::to_string(createInstanceResult));
	}

	// Create debug callback
	#ifdef _DEBUG
		VkResult createDebugMessengerResult = Proxy::vkCreateDebugUtilsMessengerEXT(m_vulkanInstance, &debugMessengerInfo, nullptr, &m_debugMessenger);
		if (createDebugMessengerResult != VK_SUCCESS)
		{
			throw std::runtime_error("Creation of debug messenger failed with error code: " + std::to_string(createDebugMessengerResult)); 
		}
	#endif
}

void VulkanApplication::CreateDisplaySurface()
{
	VkResult createSurfaceResult = glfwCreateWindowSurface(m_vulkanInstance, m_window, nullptr, &m_displaySurface);
	if (createSurfaceResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of display surface failed with error code: " + std::to_string(createSurfaceResult));
	}
}

void VulkanApplication::SelectPhysicalDevice()
{
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_vulkanInstance, &physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0)
	{
		throw std::runtime_error("Could not continue, as no Vulkan-compatible GPUs were found"); 
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(m_vulkanInstance, &physicalDeviceCount, physicalDevices.data());

	// Just learned about these, they seem pretty cool for avoiding sorting a map manually
	std::multimap<uint, VkPhysicalDevice> deviceCandidates;

	// TODO: Give user a choice between devices
	// Choose the most suitable device
	for (VkPhysicalDevice device : physicalDevices)
	{
		uint deviceScore = VkUtils::RatePhysicalDeviceCompatibility(device, m_displaySurface, m_enabledDeviceExtensions);
		deviceCandidates.insert(std::make_pair(deviceScore, device));
	}

	if (deviceCandidates.begin()->first > 0)
	{
		m_physicalDevice = deviceCandidates.begin()->second;
	}
	else
	{
		throw std::runtime_error("Could not continue, as no compatible GPUs were found"); 
	}
}

void VulkanApplication::CreateLogicalDevice()
{
	QueueFamilyIndices qfIndices = VkUtils::GetAvailableQueueFamilies(m_physicalDevice, m_displaySurface);
	if (!qfIndices.NecessaryFamiliesFilled())
	{
		throw std::runtime_error("Could not create logical device, required queue families not available");
	}

	std::vector<VkDeviceQueueCreateInfo> queueInfoList;

	// This is a set because we *cannot* have duplicate queue family indices
	std::set<uint32_t> uniqueQueueFamilies = { qfIndices.queueFamilies["graphicsQueueFamily"], qfIndices.queueFamilies["surfaceQueueFamily"] };

	float queuePriority = 1;
	for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueInfo
		{
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,		//sType
			NULL,											//pNext
			0,												//flags
			queueFamilyIndex,								//queueFamilyIndex
			1,												//queueCount
			&queuePriority									//pQueuePriorities
		};

		queueInfoList.push_back(queueInfo);
	}

	VkPhysicalDeviceFeatures featuresInfo{};

	// Validation layers
	// Vulkan no longer makes a distinction between instance-level and device-level validation layers
	// However, since the user could be using an older version of Vulkan, we still define them so as to be compatible
	uint32_t enabledLayerCount = 0;
	const char* const* enabledLayerNames = NULL;

	#ifdef _DEBUG
		enabledLayerCount = m_enabledValidationLayers.size();
		enabledLayerNames = m_enabledValidationLayers.data();
	#endif
	
	VkDeviceCreateInfo logicalDeviceInfo
	{
		VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,			//sType
    	NULL,											//pNext
    	0,												//flags
    	(uint32_t)queueInfoList.size(),					//queueCreateInfoCount
    	queueInfoList.data(),							//pQueueCreateInfos
    	enabledLayerCount,								//enabledLayerCount
    	enabledLayerNames,								//ppEnabledLayerNames
    	(uint32_t)m_enabledDeviceExtensions.size(),		//enabledExtensionCount
    	m_enabledDeviceExtensions.data(),				//ppEnabledExtensionNames
    	&featuresInfo									//pEnabledFeatures
	};

	VkResult createDeviceResult = vkCreateDevice(m_physicalDevice, &logicalDeviceInfo, nullptr, &m_logicalDevice);
	if (createDeviceResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of logical device failed with error code: " + std::to_string(createDeviceResult));
	}

	vkGetDeviceQueue(m_logicalDevice, qfIndices.queueFamilies["graphicsQueueFamily"], 0, &m_vulkanQueues["graphicsQueue"]);
	vkGetDeviceQueue(m_logicalDevice, qfIndices.queueFamilies["surfaceQueueFamily"], 0, &m_vulkanQueues["surfaceQueue"]);

}

// Public Methods
VulkanApplication::VulkanApplication()
{
    // --Init GLFW--
    if(!glfwInit()) { throw std::runtime_error("GLFW failed to initialise"); }

	// Disable OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void VulkanApplication::Init(WindowInfo winInfo, VkApplicationInfo appInfo, std::vector<const char*> vkExtensions, VkInstanceCreateFlags vkFlags)
{
    // Create window
    m_window = glfwCreateWindow(winInfo.width, winInfo.height, winInfo.title, winInfo.targetMonitor, nullptr);

	// TODO: make a callback to set this later
	glfwGetWindowSize(m_window, &m_winDimensions.x, &m_winDimensions.y);

    // --Init Vulkan--
	// Initialise vulkan, and the debug messenger if _DEBUG define is set
	CreateVulkanInstance(appInfo, vkExtensions, vkFlags);

	// Create the GLFW surface we'll be using in our swapchain
	CreateDisplaySurface();

	// Find and select a GPU to render with
	SelectPhysicalDevice();

	// Create a logical device to interface with our physical device
	CreateLogicalDevice();
}

VulkanApplication::~VulkanApplication()
{
	if (m_logicalDevice != VK_NULL_HANDLE) { vkDestroyDevice(m_logicalDevice, nullptr); }

	if (m_displaySurface != VK_NULL_HANDLE) { vkDestroySurfaceKHR(m_vulkanInstance, m_displaySurface, nullptr); }

	#ifdef _DEBUG
		if (m_debugMessenger != VK_NULL_HANDLE) { Proxy::vkDestroyDebugUtilsMessengerEXT(m_vulkanInstance, m_debugMessenger, nullptr); }
	#endif

	if (m_vulkanInstance != VK_NULL_HANDLE) { vkDestroyInstance(m_vulkanInstance, nullptr); }

	if (m_window != nullptr) { glfwDestroyWindow(m_window); }

    glfwTerminate();
}