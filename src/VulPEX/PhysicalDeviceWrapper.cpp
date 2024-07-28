#include "PhysicalDeviceWrapper.hpp"

#include <map>
#include <unordered_set>

#include "Utility/VulPEXUtils.hpp"

bool PhysicalDeviceWrapper::AreDeviceExtensionsSupported(vk::PhysicalDevice device, std::vector<const char *> extensions) const
{
	std::vector<vk::ExtensionProperties> supportedExtensions = device.enumerateDeviceExtensionProperties();

	std::unordered_set<std::string> requiredExtensions(extensions.begin(), extensions.end());

	for (vk::ExtensionProperties supportedExtensionProperties : supportedExtensions)
	{
		requiredExtensions.erase(supportedExtensionProperties.extensionName);
	}

	return requiredExtensions.empty();
}

SwapChainSupportInfo PhysicalDeviceWrapper::QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	SwapChainSupportInfo scSupportInfo;

	// Surface Capabilities
	scSupportInfo.surfaceCapabilities = device.getSurfaceCapabilitiesKHR(surface);

	// Surface Formats
	scSupportInfo.surfaceFormats = device.getSurfaceFormatsKHR(surface);

	// Present Modes
	scSupportInfo.presentModes = device.getSurfacePresentModesKHR(surface);

	return scSupportInfo;
}

uint PhysicalDeviceWrapper::RatePhysicalDeviceCompatibility(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char *> deviceExtensions)
{
	vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

	vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

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

	if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
	{
		score += 1000;
	}

	score += deviceProperties.limits.maxImageDimension2D;

	return score;
}

PhysicalDeviceWrapper::PhysicalDeviceWrapper()
{
	m_enabledDeviceExtensions = {vk::KHRSwapchainExtensionName};
}

void PhysicalDeviceWrapper::ConfigurePhysicalDevice(std::vector<const char *> deviceExtensions)
{
	m_enabledDeviceExtensions = deviceExtensions;
}

void PhysicalDeviceWrapper::SelectDevice(vk::Instance instance, vk::SurfaceKHR surface)
{
	std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();

	if (physicalDevices.size() == 0)
	{
		throw std::runtime_error("Could not continue, as no Vulkan-compatible GPUs were found"); 
	}

	// Just learned about these, they seem pretty cool for avoiding sorting a map manually
	std::multimap<uint, vk::PhysicalDevice> deviceCandidates;

	// Choose the most suitable device
	for (vk::PhysicalDevice device : physicalDevices)
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