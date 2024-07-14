#include "SwapChainWrapper.hpp"

#include <limits>
#include <algorithm>

// Private
VkSurfaceFormatKHR SwapChainWrapper::ChooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> availableFormats)
{
	for (VkSurfaceFormatKHR preferredFormat : m_preferredFormats)
	{
		for (VkSurfaceFormatKHR surfaceFormat : availableFormats)
		{
			if (surfaceFormat.format == preferredFormat.format && surfaceFormat.colorSpace == preferredFormat.colorSpace)
			{
				return surfaceFormat;
			}
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR SwapChainWrapper::ChoosePresentMode(std::vector<VkPresentModeKHR> availablePresentModes)
{
	for (VkPresentModeKHR preferredPresentMode : m_preferredPresentModes)
	{
		for (VkPresentModeKHR presentMode : availablePresentModes)
		{
			if (presentMode == preferredPresentMode)
			{
				return presentMode;
			}
		}
	}

	throw std::runtime_error("None of the preferred present modes were available");
}

VkExtent2D SwapChainWrapper::ChooseExtent(VkSurfaceCapabilitiesKHR surfaceCapabilities, GLFWwindow* window)
{
	// We need to do a conversion here from screen coordinates (what GLFW uses) and actual pixels (what Vulkan uses)
	// Sometimes, Vulkan will do this conversion for us, so we can just return the current extent
	// If that's not the case and we do actually have to do it ourself, then Vulkan will helpfully set this value to uint32_t's max value
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	VkExtent2D extent
	{
		(uint32_t)width,
		(uint32_t)height
	};

	extent.width = std::clamp(extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
	extent.height = std::clamp(extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

	return extent;
}

// Public
SwapChainWrapper::SwapChainWrapper()
{
	m_preferredFormats = {{
		VK_FORMAT_B8G8R8A8_SRGB,			//format
		VK_COLOR_SPACE_SRGB_NONLINEAR_KHR	//colorSpace
	}};

	m_preferredPresentModes = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };
}

void SwapChainWrapper::ConfigureSwapChain(std::vector<VkSurfaceFormatKHR> preferredSurfaceFormats, std::vector<VkPresentModeKHR> preferredPresentModes)
{
	m_preferredFormats = preferredSurfaceFormats;
	m_preferredPresentModes = preferredPresentModes;
}

void SwapChainWrapper::CreateSwapChain(VkDevice device, VkSurfaceKHR surface, GLFWwindow* window, SwapChainSupportInfo supportInfo, QueueFamilyIndices qfIndices)
{
	VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(supportInfo.surfaceFormats);
	VkPresentModeKHR presentMode = ChoosePresentMode(supportInfo.presentModes);
	m_extent = ChooseExtent(supportInfo.surfaceCapabilities, window);

	m_imageFormat = surfaceFormat.format;

	// Usually, we want to request one more than the minimum so that we won't have to wait for the GPU if we want to draw another image
	uint32_t imageCount = supportInfo.surfaceCapabilities.minImageCount + 1;

	// A value of 0 for maxImageCount means that there is no limit
	if (supportInfo.surfaceCapabilities.maxImageCount > 0 && imageCount > supportInfo.surfaceCapabilities.maxImageCount)
	{
		imageCount = supportInfo.surfaceCapabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapChainInfo{};
	swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.surface = surface;

	swapChainInfo.minImageCount = imageCount;
	swapChainInfo.imageFormat = m_imageFormat;
	swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainInfo.imageExtent = m_extent;
	// This is always 1 unless you're making a stereoscopic game (e.g. VR)
	swapChainInfo.imageArrayLayers = 1;
	// This is to tell Vulkan that we want to render these images straight to the screen
	// If we wanted to do something like render to a separate image first for postprocessing before displaying to the screen,
	// we could set this value to VK_IMAGE_USAGE_TRANSFER_DST_BIT
	swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t qfIndicesArray[] = { qfIndices.queueFamilies["graphicsQueueFamily"], qfIndices.queueFamilies["surfaceQueueFamily"] };
	if (qfIndicesArray[0] != qfIndicesArray[1])
	{
		// If our graphics qf and surface qf are different, we'll either need to transfer ownership on the fly, or just tell Vulkan
		// that ownership doesn't matter, and all queues can access all resources. The second is slower, but much easier to implement
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainInfo.queueFamilyIndexCount = 2;
		swapChainInfo.pQueueFamilyIndices = qfIndicesArray;
	}
	else
	{
		// If graphics qf and surface qf are the same, we can just use exclusive, and easily assign ownership of all swapchain resources to
		// the one queue family.
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainInfo.queueFamilyIndexCount = 0;
		swapChainInfo.pQueueFamilyIndices = nullptr;
	}

	swapChainInfo.preTransform = supportInfo.surfaceCapabilities.currentTransform;
	swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainInfo.presentMode = presentMode;
	// If a window covers some pixels, we'll just throw them out. This could cause problems in some niche applications, should be an option
	swapChainInfo.clipped = VK_TRUE;
	swapChainInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult createSwapChainResult = vkCreateSwapchainKHR(device, &swapChainInfo, nullptr, &m_swapChain);
	if (createSwapChainResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of swap chain failed with error code: " + std::to_string(createSwapChainResult));
	}

	uint32_t finalImageCount;
	vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(finalImageCount);
	vkGetSwapchainImagesKHR(device, m_swapChain, &imageCount, m_swapChainImages.data());
}

void SwapChainWrapper::DestroySwapChain(VkDevice device)
{
	if (m_swapChain != VK_NULL_HANDLE) { vkDestroySwapchainKHR(device, m_swapChain, nullptr); }
}
