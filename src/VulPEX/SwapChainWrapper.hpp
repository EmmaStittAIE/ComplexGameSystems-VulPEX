#pragma once

#include <vulkan/vulkan.hpp>

/**
 * This contains a VkSwapchainKHR object, as well as various info regarding swap chain settings.
 * Derive from this class if you want to create your own implementation.
*/
class SwapChainWrapper
{
	// Vulkan resources
	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;

public:
	virtual void CreateSwapChain(VkPhysicalDevice device, VkSurfaceKHR surface);
};