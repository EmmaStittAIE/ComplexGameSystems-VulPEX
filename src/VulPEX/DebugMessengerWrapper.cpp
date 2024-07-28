#ifdef _DEBUG

#include "DebugMessengerWrapper.hpp"

#include <unordered_set>

#include <Logger.hpp>

#include "Utility/VulkanExtensionProxies.hpp"

// Callbacks
// Alright, lots to unpack here
// VKAPI_ATTR is a macro placed before the return type of a function, and allows Vulkan to call the function properly using C++11 and GCC/Clang-style compilers
// VkBool32 is just a bool typedef, since Vulkan is based in C, and C doesn't have an official bool type
// VKAPI_CALL is a macro placed after the return type of a function, and allows Vulkan to call the function properly using MSVC-style compilers
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback (
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	switch(messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			Logger::Log({ pCallbackData->pMessage }, LogType::None);
			break;
		
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			Logger::Log({ pCallbackData->pMessage }, LogType::Info);
			break;
		
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			Logger::Log({ pCallbackData->pMessage }, LogType::Warning);
			break;
		
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			Logger::Log({ pCallbackData->pMessage }, LogType::Error);
			break;

		default:
			throw std::runtime_error("Debug message severity \"" + std::to_string(messageSeverity) + "\" could not be handled");
	}

	return VK_FALSE;
}

// Private Functions
bool DebugMessengerWrapper::AreValidationLayersSupported(std::vector<const char*> validationLayers)
{
	uint32_t supportedLayerCount;
	vk::enumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

	std::vector<vk::LayerProperties> supportedLayers(supportedLayerCount);
	vk::enumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

	std::unordered_set<std::string> requiredLayers(validationLayers.begin(), validationLayers.end());

	for (vk::LayerProperties supportedLayerProperties : supportedLayers)
	{
		requiredLayers.erase(supportedLayerProperties.layerName);
	}

	return requiredLayers.empty();
}

DebugMessengerWrapper::DebugMessengerWrapper()
{
	m_severitiesToLog = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	m_messageTypesToLog = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
						  vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	m_enabledValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	m_debugCallback = DebugMessageCallback;
}

// Public Functions
void DebugMessengerWrapper::ConfigureMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severitiesToLog, vk::DebugUtilsMessageTypeFlagsEXT messageTypesToLog,
											   std::vector<const char*> validationLayers)
{
	m_severitiesToLog = severitiesToLog;
	m_messageTypesToLog = messageTypesToLog;
	m_enabledValidationLayers = validationLayers;
}

void DebugMessengerWrapper::SetDebugCallback(PFN_vkDebugUtilsMessengerCallbackEXT debugCallback)
{
	m_debugCallback = debugCallback;
}

void DebugMessengerWrapper::SetUpDebugCallback()
{
	if (!AreValidationLayersSupported(m_enabledValidationLayers))
	{
		throw std::runtime_error("One or more of the validation layers specified are not supported by the target system"); 
	}

	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo(
		{},															//flags
		m_severitiesToLog,											//messageSeverity
		m_messageTypesToLog,										//messageType
		DebugMessageCallback,										//pfnUserCallback
		nullptr														//pUserData
	);

	m_debugMessengerInfo = debugMessengerInfo;
}

void DebugMessengerWrapper::LinkDebugCallback(vk::Instance instance)
{
	m_debugMessenger = instance.createDebugUtilsMessengerEXT(m_debugMessengerInfo);
}

void DebugMessengerWrapper::DestroyDebugMessenger(vk::Instance instance)
{
	if (m_debugMessenger != nullptr) { Proxy::vkDestroyDebugUtilsMessengerEXT(instance, m_debugMessenger, nullptr); }
}

#endif
