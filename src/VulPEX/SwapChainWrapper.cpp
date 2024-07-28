#include "SwapChainWrapper.hpp"

#include <limits>
#include <algorithm>

// Private
vk::SurfaceFormatKHR SwapChainWrapper::ChooseSurfaceFormat(std::vector<vk::SurfaceFormatKHR> availableFormats)
{
	for (vk::SurfaceFormatKHR preferredFormat : m_preferredFormats)
	{
		for (vk::SurfaceFormatKHR surfaceFormat : availableFormats)
		{
			if (surfaceFormat.format == preferredFormat.format && surfaceFormat.colorSpace == preferredFormat.colorSpace)
			{
				return surfaceFormat;
			}
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR SwapChainWrapper::ChoosePresentMode(std::vector<vk::PresentModeKHR> availablePresentModes)
{
	for (vk::PresentModeKHR preferredPresentMode : m_preferredPresentModes)
	{
		for (vk::PresentModeKHR presentMode : availablePresentModes)
		{
			if (presentMode == preferredPresentMode)
			{
				return presentMode;
			}
		}
	}

	throw std::runtime_error("None of the preferred present modes were available");
}

vk::Extent2D SwapChainWrapper::ChooseExtent(vk::SurfaceCapabilitiesKHR surfaceCapabilities, GLFWwindow* window)
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

	vk::Extent2D extent
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
		vk::Format::eB8G8R8A8Srgb,			//format
		vk::ColorSpaceKHR::eSrgbNonlinear	//colorSpace
	}};

	m_preferredPresentModes = { vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifo };
}

void SwapChainWrapper::ConfigureSwapChain(std::vector<vk::SurfaceFormatKHR> preferredSurfaceFormats, std::vector<vk::PresentModeKHR> preferredPresentModes)
{
	m_preferredFormats = preferredSurfaceFormats;
	m_preferredPresentModes = preferredPresentModes;
}

void SwapChainWrapper::CreateSwapChain(vk::Device device, vk::SurfaceKHR surface, GLFWwindow* window, SwapChainSupportInfo supportInfo, QueueFamilyIndices qfIndices)
{
	vk::SurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(supportInfo.surfaceFormats);
	vk::PresentModeKHR presentMode = ChoosePresentMode(supportInfo.presentModes);
	m_extent = ChooseExtent(supportInfo.surfaceCapabilities, window);

	m_imageFormat = surfaceFormat.format;

	// Usually, we want to request one more than the minimum so that we won't have to wait for the GPU if we want to draw another image
	uint32_t imageCount = supportInfo.surfaceCapabilities.minImageCount + 1;

	// A value of 0 for maxImageCount means that there is no limit
	if (supportInfo.surfaceCapabilities.maxImageCount > 0 && imageCount > supportInfo.surfaceCapabilities.maxImageCount)
	{
		imageCount = supportInfo.surfaceCapabilities.maxImageCount;
	}

	vk::SharingMode sciSharingMode;
	uint32_t sciQfIndexCount;
	const uint32_t* sciQfIndices;

	uint32_t qfIndicesArray[] = { qfIndices.queueFamilies["graphicsQueueFamily"], qfIndices.queueFamilies["surfaceQueueFamily"] };
	if (qfIndicesArray[0] != qfIndicesArray[1])
	{
		// If our graphics qf and surface qf are different, we'll either need to transfer ownership on the fly, or just tell Vulkan
		// that ownership doesn't matter, and all queues can access all resources. The second is slower, but much easier to implement
		sciSharingMode = vk::SharingMode::eConcurrent;
		sciQfIndexCount = 2;
		sciQfIndices = qfIndicesArray;
	}
	else
	{
		// If graphics qf and surface qf are the same, we can just use exclusive, and easily assign ownership of all swapchain resources to
		// the one queue family.
		sciSharingMode = vk::SharingMode::eExclusive;
		sciQfIndexCount = 0;
		sciQfIndices = nullptr;
	}

	vk::SwapchainCreateInfoKHR swapChainInfo(
		{},													//flags
		surface,											//surface
		imageCount,											//minImageCount
		m_imageFormat,										//imageFormat
		surfaceFormat.colorSpace,							//imageColorSpace 
		m_extent,											//imageExtent
		1,													//imageArrayLayers | This is always 1 unless you're making a stereoscopic game (e.g. VR)
		vk::ImageUsageFlagBits::eColorAttachment,			//imageUsage
		sciSharingMode,										//imageSharingMode
		sciQfIndexCount,									//queueFamilyIndexCount
		sciQfIndices,										//pQueueFamilyIndices  
		supportInfo.surfaceCapabilities.currentTransform,	//preTransform
		vk::CompositeAlphaFlagBitsKHR::eOpaque,				//compositeAlpha
		presentMode,										//presentMode
		vk::True,											//clipped | If a window covers some pixels, we'll just throw them out. This could cause problems in some niche applications, should be an option
		nullptr												//oldSwapchain
	);

	device.createSwapchainKHR(swapChainInfo);

	// Retrieve references to swap chain images
	m_swapChainImages = device.getSwapchainImagesKHR(m_swapChain);

	m_imageViews.resize(m_swapChainImages.size());

	for (std::size_t i = 0; i < m_swapChainImages.size(); i++)
	{
		vk::ImageViewCreateInfo imageViewInfo(
			{},												//flags
			m_swapChainImages[i],							//image
			vk::ImageViewType::e2D,							//viewType
			m_imageFormat,									//format
			{},												//components | Automatically set to identity for r, g, b, and a
			{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}	//subresourceRange
		);

		m_imageViews[i] = device.createImageView(imageViewInfo);
	}
}

void SwapChainWrapper::DestroySwapChain(vk::Device device)
{
	if (m_imageViews.size() != 0)
	{
		for (vk::ImageView imageView : m_imageViews)
		{
			device.destroyImageView(imageView);
		}
	}

	if (m_swapChain != nullptr) { device.destroySwapchainKHR(m_swapChain); }
}
