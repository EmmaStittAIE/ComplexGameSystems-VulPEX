#pragma once

#include <vulkan/vulkan.hpp>

struct SwapChainSupportInfo
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
};

class PhysicalDeviceWrapper
{
	// Vulkan resources
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	// Misc resources
	SwapChainSupportInfo m_supportInfo;

	std::vector<const char*> m_enabledDeviceExtensions;

	// Functions
	bool AreDeviceExtensionsSupported(VkPhysicalDevice device, std::vector<const char*> extensions) const;
	SwapChainSupportInfo QuerySwapChainCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface);

	uint RatePhysicalDeviceCompatibility(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char *> deviceExtensions);

public:
	PhysicalDeviceWrapper();

	void ConfigurePhysicalDevice(std::vector<const char*> deviceExtensions);
	void SelectDevice(VkInstance instance, VkSurfaceKHR surface);

	// Getters
	VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; };
	SwapChainSupportInfo GetSwapChainSupportInfo() const { return m_supportInfo; };
	std::vector<const char*> GetDeviceExtensions() const { return m_enabledDeviceExtensions; };
};