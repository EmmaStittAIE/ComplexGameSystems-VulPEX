#pragma once

#include <unordered_map>

#include "Utility/VulkanDynamicInclude.hpp"

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
	vk::Device m_logicalDevice = nullptr;

	// Misc resources
	std::unordered_map<std::string, vk::Queue> m_requestedQueues;

	QueueFamilyIndices m_qfIndices;

	// Functions
	QueueFamilyIndices GetAvailableQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);

public:
	LogicalDeviceWrapper();

	void ConfigureLogicalDevice(std::unordered_map<std::string, vk::Queue> queues);
	
	#ifdef _DEBUG
		void CreateLogicalDevice(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char*> deviceExtensions, std::vector<const char*> validationLayers);
	#else
		void CreateLogicalDevice(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char*> deviceExtensions);
	#endif

	// Getters
	vk::Device GetLogicalDevice() const { return m_logicalDevice; };
	vk::Queue GetQueue(std::string key) const { return m_requestedQueues.at(key); };
	QueueFamilyIndices GetQueueFamilyIndices() const { return m_qfIndices; };

	// Cleanup
	void DestroyLogicalDevice();
};