#include "VulkanApplication.hpp"

#include <vector>
#include <iostream>

#include <Logger.hpp>

#include "VulkanExtensionProxies.hpp"

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

std::vector<const char *> VulkanApplication::GetRequiredExtensions() const
{
	// Retrieve glfw's list of required extensions
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	#ifdef _DEBUG
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif

	return requiredExtensions;
}

bool VulkanApplication::AreExtensionsSupported(std::vector<const char *> extensions) const
{
	// Get extension compatibility info

	// First, find out how many supported extensions there are
	uint32_t supportedExtensionsCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, nullptr);

	// Create an array to store the actual info about those extensions
	std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);

	// Retrieve the info on all supported extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, supportedExtensions.data());

	// Finally, check if these extensions are supported by the system
	for (const char* extensionName : extensions)
	{
		bool extensionSupported = false;

		for (VkExtensionProperties supportedExtensionProperties : supportedExtensions)
		{
			if (strcmp(supportedExtensionProperties.extensionName, extensionName) == 0)
			{
				extensionSupported = true;
				break;
			}
		}
		
		if (!extensionSupported)
		{
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
	bool VulkanApplication::AreValidationLayersSupported(std::vector<const char*> validationLayers) const
	{
		uint32_t supportedLayerCount;
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

		std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

		// Finally, check if these extensions are supported by the system
		for (const char* layerName : validationLayers)
		{
			bool layerSupported = false;

			for (VkLayerProperties supportedLayerProperties : supportedLayers)
			{
				if (strcmp(supportedLayerProperties.layerName, layerName) == 0)
				{
					layerSupported = true;
					break;
				}
			}
			
			if (!layerSupported)
			{
				return false;
			}
		}

		return true;
	}
#endif
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

	glfwGetWindowSize(m_window, &m_winDimensions.x, &m_winDimensions.y);

    // --Init Vulkan--
	// Get Extension Info

	std::vector<const char*> requiredExtensions = GetRequiredExtensions();

	// Add reqired extensions to vkExtensions
	vkExtensions.reserve(vkExtensions.size() + requiredExtensions.size());
	vkExtensions.insert(vkExtensions.end(), requiredExtensions.begin(), requiredExtensions.end());

	if (!AreExtensionsSupported(vkExtensions))
	{
		throw std::runtime_error("One or more of the extensions specified are not supported by the target system"); 
	}

	// Validation layers
	uint32_t enabledLayerCount = 0;
	const char* const* enabledLayerNames = NULL;
	const VkDebugUtilsMessengerCreateInfoEXT* nextPointer = NULL;

	#ifdef _DEBUG
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

		if (!AreValidationLayersSupported(validationLayers))
		{
			throw std::runtime_error("One or more of the validation layers specified are not supported by the target system"); 
		}

		enabledLayerCount = validationLayers.size();
		enabledLayerNames = validationLayers.data();

		// Only calls to warning and error will be passed through
		// This needs to be set up now so we can show debug messages for creation and destruction of an instance, even though our debug messenger will be gone
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo
		{
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,											//sType
			NULL,																								//pNext
			0,																									//flags
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,	//messageSeverity
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT,														//messageType
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

VulkanApplication::~VulkanApplication()
{
	#ifdef _DEBUG
		if (m_debugMessenger != NULL) { Proxy::vkDestroyDebugUtilsMessengerEXT(m_vulkanInstance, m_debugMessenger, nullptr); }
	#endif

	if (m_vulkanInstance != NULL) { vkDestroyInstance(m_vulkanInstance, nullptr); }

	if (m_window != nullptr) { glfwDestroyWindow(m_window); }

    glfwTerminate();
}