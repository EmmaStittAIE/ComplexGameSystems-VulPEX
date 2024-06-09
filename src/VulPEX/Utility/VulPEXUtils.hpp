#include <vector>

#include <vulkan/vulkan.hpp>

#include "VulPEXStructs.hpp"

namespace VkUtils
{
	extern std::vector<const char*> GetRequiredExtensions();
	extern QueueFamilyIndices GetAvailableQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	extern SwapChainSupportInfo QuerySwapChainCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface);

	extern uint RatePhysicalDeviceCompatibility(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char *> deviceExtensions);

	// Bool functions
	extern bool AreInstanceExtensionsSupported(std::vector<const char*> extensions);
	extern bool AreDeviceExtensionsSupported(VkPhysicalDevice device, std::vector<const char*> extensions);
	#ifdef _DEBUG
		extern bool AreValidationLayersSupported(std::vector<const char*> validationLayers);
	#endif


}