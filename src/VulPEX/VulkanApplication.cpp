#include "VulkanApplication.hpp"

#include <vector>
#include <iostream>

// Callbacks
// Alright, lots to unpack here
// VKAPI_ATTR is a macro placed before the return type of a function, and allows Vulkan to call the function properly using C++11 and GCC/Clang-style compilers
// VkBool32 is just a bool typedef, since Vulkan is based in C, and C doesn't have an official bool type
// VKAPI_CALL is a macro placed after the return type of a function, and allows Vulkan to call the function properly using MSVC-style compilers
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	// TODO: Add proper logging
}

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

	// TODO: figure out why this symbol doesn't exist
	#ifdef _DEBUG
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

		if (!AreValidationLayersSupported(validationLayers))
		{
			throw std::runtime_error("One or more of the validation layers specified are not supported by the target system"); 
		}

		enabledLayerCount = validationLayers.size();
		enabledLayerNames = validationLayers.data();
	#endif

	// Configure Instance Info
	VkInstanceCreateInfo instanceInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	//sType
		NULL,									//pNext
		vkFlags,								//flags
		&appInfo,								//pApplicationInfo
		enabledLayerCount,						//enabledLayerCount
		enabledLayerNames,						//ppEnabledLayerNames
		(uint32_t)vkExtensions.size(),			//enabledExtensionCount
		vkExtensions.data()						//ppEnabledExtensionNames
	};

	// Create info, custom memory allocator callback, pointer to instance
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vulkanInstance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of Vulkan instance failed with error code: " + std::to_string(result));
	}
}

VulkanApplication::~VulkanApplication()
{
	if (vulkanInstance != NULL) { vkDestroyInstance(vulkanInstance, nullptr); }

	if (m_window != nullptr) { glfwDestroyWindow(m_window); }

    glfwTerminate();
}