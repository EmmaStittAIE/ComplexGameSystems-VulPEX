#pragma once

#include <vector>

#include "VulkanDynamicInclude.hpp"

namespace VkUtils
{
	extern std::vector<const char*> GetRequiredExtensions();

	// Bool functions
	extern bool AreInstanceExtensionsSupported(std::vector<const char*> extensions);
}