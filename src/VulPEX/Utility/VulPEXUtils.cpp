#include "VulPEXUtils.hpp"

#include <unordered_set>

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

std::vector<const char*> VkUtils::GetRequiredExtensions()
{
	// Retrieve glfw's list of required extensions
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	#ifdef _DEBUG
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif

	return requiredExtensions;
}

bool VkUtils::AreInstanceExtensionsSupported(std::vector<const char *> extensions)
{
	// Get extension compatibility info

	// First, find out how many supported extensions there are
	uint32_t supportedExtensionsCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, nullptr);

	// Then, retrieve the info on all supported extensions
	std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, supportedExtensions.data());

	// Finally, check if these extensions are supported by the system
	std::unordered_set<std::string> requiredExtensions(extensions.begin(), extensions.end());

	for (VkExtensionProperties supportedExtensionProperties : supportedExtensions)
	{
		requiredExtensions.erase(supportedExtensionProperties.extensionName);
	}

	return requiredExtensions.empty();
}
