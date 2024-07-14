#pragma once

#include <unordered_map>

#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices
{
	// TODO: later, optimise queue use by using specialised queues (i.e async transfer queues)
	/**
	 * Queue families aren't concrete things, so there's no real concrete names I can give them in an Enum.
	 * Because of this, I've left the key as a string that the user can define.
	 * There are, however, some default/preferred names that the internal library code uses.
	 * They are as follows:
	 *  - graphicsQueueFamily
	 *  - surfaceQueueFamily
	*/
	std::unordered_map<std::string, uint32_t> queueFamilies;

	// TODO: Make this only contain the families that the user has specified are crucial to the project running
	bool NecessaryFamiliesFilled() const
	{
		return queueFamilies.contains("graphicsQueueFamily") && queueFamilies.contains("surfaceQueueFamily");
	}

	bool IsFilled() const
	{
		return queueFamilies.contains("graphicsQueueFamily") && queueFamilies.contains("surfaceQueueFamily");
	}
};

class LogicalDeviceWrapper
{
	// Vulkan resources
	VkDevice m_logicalDevice = VK_NULL_HANDLE;

	// Misc resources
	std::unordered_map<std::string, VkQueue> m_requestedQueues;

	QueueFamilyIndices m_qfIndices;

	// Functions
	QueueFamilyIndices GetAvailableQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

public:
	LogicalDeviceWrapper();

	void ConfigureLogicalDevice(std::unordered_map<std::string, VkQueue> queues);
	
	#ifdef _DEBUG
		void CreateLogicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions, std::vector<const char*> validationLayers);
	#else
		void CreateLogicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions);
	#endif

	// Getters
	VkDevice GetLogicalDevice() const { return m_logicalDevice; };
	QueueFamilyIndices GetQueueFamilyIndices() const { return m_qfIndices; };

	// Cleanup
	void DestroyLogicalDevice();
};