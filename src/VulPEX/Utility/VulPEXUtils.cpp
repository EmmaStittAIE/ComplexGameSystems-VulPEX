#include "VulPEXUtils.hpp"

#include <unordered_set>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

std::vector<const char*> VkUtils::GetRequiredExtensions()
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

QueueFamilyIndices VkUtils::GetAvailableQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// TODO: this feels icky, too many strange caveats
	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.queueFamilies.insert_or_assign("graphicsQueueFamily", i);
		}

		VkBool32 surfaceSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);

		if (surfaceSupport)
		{
			indices.queueFamilies.insert_or_assign("surfaceQueueFamily", i);
		}

		if (indices.IsFilled()) { break; }
	}

	return indices;
}

SwapChainSupportInfo VkUtils::QuerySwapChainCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	// Surface Capabilities
	SwapChainSupportInfo supportInfo;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportInfo.surfaceCapabilities);

	// Surface Formats
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, nullptr);

	if (surfaceFormatCount != 0)
	{
		// We resize here because the following function requires an array of empty initialised structs, not just reserved memory
		supportInfo.surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, supportInfo.surfaceFormats.data());
	}

	// Present Modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (surfaceFormatCount != 0)
	{
		supportInfo.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, supportInfo.presentModes.data());
	}

	return supportInfo;
}

// TODO: Add more things to influence score
uint VkUtils::RatePhysicalDeviceCompatibility(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char *> deviceExtensions)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// Fail states
	if (!AreDeviceExtensionsSupported(device, deviceExtensions))
	{
		return 0;
	}

    if (!deviceFeatures.geometryShader)
	{
        return 0;
    }

	// This must occur after we've confirmed that deviceExtensions are supported
	SwapChainSupportInfo scSupportInfo = QuerySwapChainCapabilities(device, surface);
	if (scSupportInfo.surfaceFormats.empty() || scSupportInfo.presentModes.empty())
	{
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

bool VkUtils::AreInstanceExtensionsSupported(std::vector<const char *> extensions)
{
	// Get extension compatibility info

	// First, find out how many supported extensions there are
	uint32_t supportedExtensionsCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, nullptr);

	// Then, retrieve the info on all supported extensions
	std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, supportedExtensions.data());

	// Finally, check if these extensions are supported by the system
	std::unordered_set<std::string> requiredExtensions(extensions.begin(), extensions.end());

	for (VkExtensionProperties supportedExtensionProperties : supportedExtensions)
	{
		requiredExtensions.erase(supportedExtensionProperties.extensionName);
	}

	return requiredExtensions.empty();
}

bool VkUtils::AreDeviceExtensionsSupported(VkPhysicalDevice device, std::vector<const char *> extensions)
{
	uint32_t supportedExtensionsCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedExtensionsCount, nullptr);

	std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedExtensionsCount, supportedExtensions.data());

	std::unordered_set<std::string> requiredExtensions(extensions.begin(), extensions.end());

	for (VkExtensionProperties supportedExtensionProperties : supportedExtensions)
	{
		requiredExtensions.erase(supportedExtensionProperties.extensionName);
	}

	return requiredExtensions.empty();
}

#ifdef _DEBUG
	bool VkUtils::AreValidationLayersSupported(std::vector<const char*> validationLayers)
	{
		uint32_t supportedLayerCount;
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

		std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
		vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

		std::unordered_set<std::string> requiredLayers(validationLayers.begin(), validationLayers.end());

		for (VkLayerProperties supportedLayerProperties : supportedLayers)
		{
			requiredLayers.erase(supportedLayerProperties.layerName);
		}

		return requiredLayers.empty();
	}
#endif
