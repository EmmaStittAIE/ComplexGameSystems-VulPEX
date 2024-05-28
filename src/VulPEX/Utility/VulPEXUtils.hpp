#include <vector>

#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsQueueFamily;

	// TODO: Make this only contain the families that the user has specified are crucial to the project running
	bool NecessaryFamiliesFilled() const
	{
		return graphicsQueueFamily.has_value();
	}

	bool IsFilled() const
	{
		return graphicsQueueFamily.has_value();
	}
};

namespace VkUtils
{
	extern std::vector<const char *> GetRequiredExtensions();
	extern QueueFamilyIndices GetAvailableQueueFamilies(VkPhysicalDevice device);

	extern uint RatePhysicalDeviceCompatibility(VkPhysicalDevice device);

	// Bool functions
	extern bool AreExtensionsSupported(std::vector<const char*> extensions);
	#ifdef _DEBUG
		extern bool AreValidationLayersSupported(std::vector<const char*> validationLayers);
	#endif


}