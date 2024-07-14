#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "PhysicalDeviceWrapper.hpp"
#include "LogicalDeviceWrapper.hpp"

class SwapChainWrapper
{
	// Vulkan resources
	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> m_swapChainImages;

	VkFormat m_imageFormat;
	VkExtent2D m_extent;

	std::vector<VkSurfaceFormatKHR> m_preferredFormats;
	std::vector<VkPresentModeKHR> m_preferredPresentModes;

	// Functions
	VkSurfaceFormatKHR ChooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats);
	VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR surfaceCapabilities, GLFWwindow* window);

public:
	SwapChainWrapper();

	// Surface formats and present modes should be ordered in order of preference, from most preferred to least preferred
	void ConfigureSwapChain(std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats, std::vector<VkPresentModeKHR> preferredPresentModes);
	void CreateSwapChain(VkDevice device, VkSurfaceKHR surface, GLFWwindow* window, SwapChainSupportInfo supportInfo, QueueFamilyIndices qfIndices);

	// Getters
	std::vector<VkImage> GetSwapChainImages() const { return m_swapChainImages; };

	// Cleanup
	void DestroySwapChain(VkDevice device);
};