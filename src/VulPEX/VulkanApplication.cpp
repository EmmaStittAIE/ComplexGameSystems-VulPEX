#include "VulkanApplication.hpp"

#include <vector>
#include <iostream>

// Private Methods

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

	// Retrieve glfw's list of required extensions
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add reqired extensions to vkExtensions
	for (uint i = 0; i < glfwExtensionCount; i++)
	{
		// If the user hasn't already, add the required extensions to our extension list
		if (std::find(vkExtensions.begin(), vkExtensions.end(), glfwExtensions[i]) == vkExtensions.end())
		{
			vkExtensions.push_back(glfwExtensions[i]);
		}
	}

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

	std::cout << std::to_string(enabledLayerCount) << std::endl;
	std::cout << enabledLayerNames << std::endl;

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