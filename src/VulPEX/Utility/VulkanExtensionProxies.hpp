#pragma once

#include <vulkan/vulkan.hpp>

namespace Proxy
{
	// Copied from https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
	// It works well, and I wouldn't know how to make it better, so I just took this one wholecloth
	#ifdef _DEBUG
		VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
											VkDebugUtilsMessengerEXT* pDebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr) {
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			} else {
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}
	#endif

	// Same as above
	#ifdef _DEBUG
		void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr) {
				func(instance, debugMessenger, pAllocator);
			}
		}
	#endif

	// Made this one myself to test my debug logging, based on the above
	void vkSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
									  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
	{
		auto func = (PFN_vkSubmitDebugUtilsMessageEXT) vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
		if (func != nullptr) {
			func(instance, messageSeverity, messageTypes, pCallbackData);
		}
	}
}