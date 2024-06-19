#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "VulPEXStructs.hpp"

namespace VkUtils
{
	extern std::vector<const char*> GetRequiredExtensions();

	// Bool functions
	extern bool AreInstanceExtensionsSupported(std::vector<const char*> extensions);
}