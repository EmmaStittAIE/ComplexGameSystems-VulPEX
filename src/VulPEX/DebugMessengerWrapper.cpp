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
	vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

	std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
	vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

	std::unordered_set<std::string> requiredLayers(validationLayers.begin(), validationLayers.end());

	for (VkLayerProperties supportedLayerProperties : supportedLayers)
	{
		requiredLayers.erase(supportedLayerProperties.layerName);
	}

	return requiredLayers.empty();
}

DebugMessengerWrapper::DebugMessengerWrapper()
{
	m_severitiesToLog = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	m_messageTypesToLog = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
						  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	m_enabledValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	m_debugCallback = DebugMessageCallback;
}

// Public Functions
void DebugMessengerWrapper::ConfigureMessenger(VkDebugUtilsMessageSeverityFlagsEXT severitiesToLog, VkDebugUtilsMessageTypeFlagsEXT messageTypesToLog,
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

	VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo
	{
		VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,	//sType
		NULL,														//pNext
		0,															//flags
		m_severitiesToLog,											//messageSeverity
		m_messageTypesToLog,										//messageType
		DebugMessageCallback,										//pfnUserCallback
		nullptr														//pUserData
	};

	m_debugMessengerInfo = debugMessengerInfo;
}

void DebugMessengerWrapper::LinkDebugCallback(VkInstance instance)
{
	// Create debug callback
	VkResult createDebugMessengerResult = Proxy::vkCreateDebugUtilsMessengerEXT(instance, &m_debugMessengerInfo, nullptr, &m_debugMessenger);
	if (createDebugMessengerResult != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of debug messenger failed with error code: " + std::to_string(createDebugMessengerResult)); 
	}
}

void DebugMessengerWrapper::DestroyDebugMessenger(VkInstance instance)
{
	if (m_debugMessenger != VK_NULL_HANDLE) { Proxy::vkDestroyDebugUtilsMessengerEXT(instance, m_debugMessenger, nullptr); }
}

#endif
