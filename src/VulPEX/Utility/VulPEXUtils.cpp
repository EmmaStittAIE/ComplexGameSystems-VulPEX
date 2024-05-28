#include "VulPEXUtils.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

std::vector<const char *> VkUtils::GetRequiredExtensions()
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

QueueFamilyIndices VkUtils::GetAvailableQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsQueueFamily = i;
		}

		if (indices.IsFilled()) { break; }
	}

	return indices;
}

// TODO: Add more things to influence score
uint VkUtils::RatePhysicalDeviceCompatibility(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices qfIndices = VkUtils::GetAvailableQueueFamilies(device);
	if (!qfIndices.NecessaryFamiliesFilled())
	{
		throw std::runtime_error("Could not create logical device, required queue families not available");
	}

	// Fail states
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

	int score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	return score;
}

bool VkUtils::AreExtensionsSupported(std::vector<const char *> extensions)
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
	bool VkUtils::AreValidationLayersSupported(std::vector<const char*> validationLayers)
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
