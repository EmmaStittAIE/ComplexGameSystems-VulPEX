#pragma once

#include "Utility/VulkanDynamicInclude.hpp"

struct SwapChainSupportInfo
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<vk::SurfaceFormatKHR> surfaceFormats;
	std::vector<vk::PresentModeKHR> presentModes;
};

class PhysicalDeviceWrapper
{
	// Vulkan resources
	vk::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

	// Misc resources
	SwapChainSupportInfo m_supportInfo;

	std::vector<const char*> m_enabledDeviceExtensions;

	// Functions
	bool AreDeviceExtensionsSupported(vk::PhysicalDevice device, std::vector<const char*> extensions) const;
	SwapChainSupportInfo QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	uint RatePhysicalDeviceCompatibility(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char *> deviceExtensions);

public:
	PhysicalDeviceWrapper();

	void ConfigurePhysicalDevice(std::vector<const char*> deviceExtensions);
	void SelectDevice(vk::Instance instance, vk::SurfaceKHR surface);

	// Getters
	vk::PhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; };
	SwapChainSupportInfo GetSwapChainSupportInfo() const { return m_supportInfo; };
	std::vector<const char*> GetDeviceExtensions() const { return m_enabledDeviceExtensions; };
};