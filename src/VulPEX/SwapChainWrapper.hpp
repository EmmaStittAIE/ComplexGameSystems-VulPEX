#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include "PhysicalDeviceWrapper.hpp"
#include "LogicalDeviceWrapper.hpp"

class SwapChainWrapper
{
	// Vulkan resources
	vk::SwapchainKHR m_swapChain = VK_NULL_HANDLE;
	std::vector<vk::Image> m_swapChainImages;
	std::vector<vk::ImageView> m_imageViews;
	std::vector<vk::Framebuffer> m_frameBuffers;

	vk::Format m_imageFormat;
	vk::Extent2D m_extent;

	std::vector<vk::SurfaceFormatKHR> m_preferredFormats;
	std::vector<vk::PresentModeKHR> m_preferredPresentModes;

	// Functions
	vk::SurfaceFormatKHR ChooseSurfaceFormat(std::vector<vk::SurfaceFormatKHR> availableFormats);
	vk::PresentModeKHR ChoosePresentMode(std::vector<vk::PresentModeKHR> availablePresentModes);
	vk::Extent2D ChooseExtent(vk::SurfaceCapabilitiesKHR surfaceCapabilities, GLFWwindow* window);

public:
	SwapChainWrapper();

	// Surface formats and present modes should be ordered in order of preference, from most preferred to least preferred
	void ConfigureSwapChain(std::vector<vk::SurfaceFormatKHR> preferredSurfaceFormats, std::vector<vk::PresentModeKHR> preferredPresentModes);
	void CreateSwapChain(vk::Device device, vk::SurfaceKHR surface, GLFWwindow* window, SwapChainSupportInfo supportInfo, QueueFamilyIndices qfIndices);
	void CreateFramebuffers(vk::Device device, vk::RenderPass renderPass);

	// Getters
	vk::SwapchainKHR GetSwapchain() const { return m_swapChain; };
	vk::Format GetFormat() const { return m_imageFormat; };
	vk::Extent2D GetExtent() const { return m_extent; };
	vk::Framebuffer GetFramebuffer(uint32_t index) const { return m_frameBuffers[index]; };

	std::vector<vk::Image> GetSwapChainImages() const { return m_swapChainImages; };
	std::vector<vk::Framebuffer> GetFramebufferVector() const { return m_frameBuffers; };

	// Cleanup
	void DestroySwapChain(vk::Device device);
};