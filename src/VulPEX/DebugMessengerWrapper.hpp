#ifdef _DEBUG

#pragma once

#include "Utility/VulkanDynamicInclude.hpp"

class DebugMessengerWrapper
{
	// Vulkan resources
	vk::DebugUtilsMessengerEXT m_debugMessenger = nullptr;

	// Misc resources
	vk::DebugUtilsMessengerCreateInfoEXT m_debugMessengerInfo;

	vk::DebugUtilsMessageSeverityFlagsEXT m_severitiesToLog;

	vk::DebugUtilsMessageTypeFlagsEXT m_messageTypesToLog;

	std::vector<const char*> m_enabledValidationLayers;

	PFN_vkDebugUtilsMessengerCallbackEXT m_debugCallback;

	// Functions
	bool AreValidationLayersSupported(std::vector<const char*> validationLayers);

public:
	DebugMessengerWrapper();

	// Must be called before SetUpDebugCallback
	void ConfigureMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severitiesToLog, vk::DebugUtilsMessageTypeFlagsEXT messageTypesToLog,
							std::vector<const char*> validationLayers);
	void SetDebugCallback(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback);

	// Must be called before LinkDebugCallback
	void SetUpDebugCallback();
	void LinkDebugCallback(vk::Instance instance);

	// Getters
	std::vector<const char*> GetValidationLayers() const { return m_enabledValidationLayers; };
	const vk::DebugUtilsMessengerCreateInfoEXT* GetDebugMessengerInfoPointer() const { return &m_debugMessengerInfo; };

	// Cleanup
	void DestroyDebugMessenger(vk::Instance instance);
};

#endif
