#ifdef _DEBUG

#pragma once

#include <vulkan/vulkan.hpp>

class DebugMessengerWrapper
{
	// Vulkan resources
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

	// Misc resources
	VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerInfo;

	VkDebugUtilsMessageSeverityFlagsEXT m_severitiesToLog;

	VkDebugUtilsMessageTypeFlagsEXT m_messageTypesToLog;

	std::vector<const char*> m_enabledValidationLayers;

	PFN_vkDebugUtilsMessengerCallbackEXT m_debugCallback;

	// Functions
	bool AreValidationLayersSupported(std::vector<const char*> validationLayers);

public:
	DebugMessengerWrapper();

	// Must be called before SetUpDebugCallback
	void ConfigureMessenger(VkDebugUtilsMessageSeverityFlagsEXT severitiesToLog, VkDebugUtilsMessageTypeFlagsEXT messageTypesToLog,
							std::vector<const char*> validationLayers);
	void SetDebugCallback(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);

	// Must be called before LinkDebugCallback
	void SetUpDebugCallback();
	void LinkDebugCallback(VkInstance instance);

	// Getters
	std::vector<const char*> GetValidationLayers() const { return m_enabledValidationLayers; };
	const VkDebugUtilsMessengerCreateInfoEXT* GetDebugMessengerInfoPointer() const { return &m_debugMessengerInfo; };

	// Cleanup
	void DestroyDebugMessenger(VkInstance instance);
};

#endif
