#include "PhysicalDeviceWrapper.hpp"

#include <map>
#include <unordered_set>

#include "Utility/VulPEXUtils.hpp"

bool PhysicalDeviceWrapper::AreDeviceExtensionsSupported(VkPhysicalDevice device, std::vector<const char *> extensions) const
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

SwapChainSupportInfo PhysicalDeviceWrapper::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportInfo scSupportInfo;

	// Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &scSupportInfo.surfaceCapabilities);

	// Surface Formats
	uint32_t surfaceFormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, nullptr);

	if (surfaceFormatCount != 0)
	{
		// We resize here because the following function requires an array of empty initialised structs, not just reserved memory
		scSupportInfo.surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, scSupportInfo.surfaceFormats.data());
	}

	// Present Modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		scSupportInfo.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, scSupportInfo.presentModes.data());
	}

	return scSupportInfo;
}

uint PhysicalDeviceWrapper::RatePhysicalDeviceCompatibility(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char *> deviceExtensions)
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
	m_supportInfo = QuerySwapChainSupport(device, surface);
	if (m_supportInfo.surfaceFormats.empty() || m_supportInfo.presentModes.empty())
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

PhysicalDeviceWrapper::PhysicalDeviceWrapper()
{
	m_enabledDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

void PhysicalDeviceWrapper::ConfigurePhysicalDevice(std::vector<const char *> deviceExtensions)
{
	m_enabledDeviceExtensions = deviceExtensions;
}

void PhysicalDeviceWrapper::SelectDevice(VkInstance instance, VkSurfaceKHR surface)
{
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0)
	{
		throw std::runtime_error("Could not continue, as no Vulkan-compatible GPUs were found"); 
	}

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	// Just learned about these, they seem pretty cool for avoiding sorting a map manually
	std::multimap<uint, VkPhysicalDevice> deviceCandidates;

	// Choose the most suitable device
	for (VkPhysicalDevice device : physicalDevices)
	{
		uint deviceScore = RatePhysicalDeviceCompatibility(device, surface, m_enabledDeviceExtensions);
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