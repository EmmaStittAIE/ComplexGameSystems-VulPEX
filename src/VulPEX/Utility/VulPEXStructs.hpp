#include <unordered_map>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices
{
	// TODO: later, optimise queue use by using specialised queues (i.e async transfer queues)
	/**
	 * Queue families aren't concrete things, so there's no real concrete names I can give them in an Enum.
	 * Because of this, I've left the key as a string that the user can define.
	 * There are, however, some default/preferred names that the internal library code uses.
	 * They are as follows:
	 *  - graphicsQueueFamily
	 *  - surfaceQueueFamily
	*/
	std::unordered_map<std::string, uint32_t> queueFamilies;

	// TODO: Make this only contain the families that the user has specified are crucial to the project running
	bool NecessaryFamiliesFilled() const
	{
		return queueFamilies.contains("graphicsQueueFamily") && queueFamilies.contains("surfaceQueueFamily");
	}

	bool IsFilled() const
	{
		return queueFamilies.contains("graphicsQueueFamily") && queueFamilies.contains("surfaceQueueFamily");
	}
};

struct SwapChainSupportInfo
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
};