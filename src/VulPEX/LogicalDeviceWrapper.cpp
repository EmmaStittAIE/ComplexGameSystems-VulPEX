#include "LogicalDeviceWrapper.hpp"

#include <set>

// Private
QueueFamilyIndices LogicalDeviceWrapper::GetAvailableQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// TODO: this feels icky, too many strange caveats
	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.queueFamilies.insert_or_assign("graphicsQueueFamily", i);
		}

		VkBool32 surfaceSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceSupport);

		if (surfaceSupport)
		{
			indices.queueFamilies.insert_or_assign("surfaceQueueFamily", i);
		}

		if (indices.IsFilled()) { break; }
	}

	return indices;
}

LogicalDeviceWrapper::LogicalDeviceWrapper()
{
	m_requestedQueues = {{"graphicsQueue", VK_NULL_HANDLE}, {"surfaceQueue", VK_NULL_HANDLE}};
}

void LogicalDeviceWrapper::ConfigureLogicalDevice(std::unordered_map<std::string, VkQueue> queues)
{
	m_requestedQueues = queues;
}

// Public
#ifdef _DEBUG
	void LogicalDeviceWrapper::CreateLogicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions, std::vector<const char*> validationLayers)
	{
		QueueFamilyIndices qfIndices = GetAvailableQueueFamilies(device, surface);
		if (!qfIndices.NecessaryFamiliesFilled())
		{
			throw std::runtime_error("Could not create logical device, required queue families not available");
		}

		std::vector<VkDeviceQueueCreateInfo> queueInfoList;

		// This is a set because we *cannot* have duplicate queue family indices
		std::set<uint32_t> uniqueQueueFamilies = { qfIndices.queueFamilies["graphicsQueueFamily"], qfIndices.queueFamilies["surfaceQueueFamily"] };

		float queuePriority = 1;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueInfo
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,		//sType
				NULL,											//pNext
				0,												//flags
				queueFamilyIndex,								//queueFamilyIndex
				1,												//queueCount
				&queuePriority									//pQueuePriorities
			};

			queueInfoList.push_back(queueInfo);
		}

		VkPhysicalDeviceFeatures featuresInfo{};

		// Validation layers
		// Vulkan no longer makes a distinction between instance-level and device-level validation layers
		// However, since the user could be using an older version of Vulkan, we still define them so as to be compatible
		uint32_t enabledLayerCount = 0;
		const char* const* enabledLayerNames = NULL;

		#ifdef _DEBUG
			enabledLayerCount = validationLayers.size();
			enabledLayerNames = validationLayers.data();
		#endif
		
		VkDeviceCreateInfo logicalDeviceInfo
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,			//sType
			NULL,											//pNext
			0,												//flags
			(uint32_t)queueInfoList.size(),					//queueCreateInfoCount
			queueInfoList.data(),							//pQueueCreateInfos
			enabledLayerCount,								//enabledLayerCount
			enabledLayerNames,								//ppEnabledLayerNames
			(uint32_t)deviceExtensions.size(),		//enabledExtensionCount
			deviceExtensions.data(),				//ppEnabledExtensionNames
			&featuresInfo									//pEnabledFeatures
		};

		VkResult createDeviceResult = vkCreateDevice(device, &logicalDeviceInfo, nullptr, &m_logicalDevice);
		if (createDeviceResult != VK_SUCCESS)
		{
			throw std::runtime_error("Creation of logical device failed with error code: " + std::to_string(createDeviceResult));
		}

		vkGetDeviceQueue(m_logicalDevice, qfIndices.queueFamilies["graphicsQueueFamily"], 0, &m_requestedQueues["graphicsQueue"]);
		vkGetDeviceQueue(m_logicalDevice, qfIndices.queueFamilies["surfaceQueueFamily"], 0, &m_requestedQueues["surfaceQueue"]);
	}
#else
	void LogicalDeviceWrapper::CreateLogicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface, std::vector<const char*> deviceExtensions)
	{
		QueueFamilyIndices qfIndices = GetAvailableQueueFamilies(device, surface);
		if (!qfIndices.NecessaryFamiliesFilled())
		{
			throw std::runtime_error("Could not create logical device, required queue families not available");
		}

		std::vector<VkDeviceQueueCreateInfo> queueInfoList;

		// This is a set because we *cannot* have duplicate queue family indices
		std::set<uint32_t> uniqueQueueFamilies = { qfIndices.queueFamilies["graphicsQueueFamily"], qfIndices.queueFamilies["surfaceQueueFamily"] };

		float queuePriority = 1;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueInfo
			{
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,		//sType
				NULL,											//pNext
				0,												//flags
				queueFamilyIndex,								//queueFamilyIndex
				1,												//queueCount
				&queuePriority									//pQueuePriorities
			};

			queueInfoList.push_back(queueInfo);
		}

		VkPhysicalDeviceFeatures featuresInfo{};

		VkDeviceCreateInfo logicalDeviceInfo
		{
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,			//sType
			NULL,											//pNext
			0,												//flags
			(uint32_t)queueInfoList.size(),					//queueCreateInfoCount
			queueInfoList.data(),							//pQueueCreateInfos
			0,												//enabledLayerCount
			NULL,											//ppEnabledLayerNames
			(uint32_t)deviceExtensions.size(),				//enabledExtensionCount
			deviceExtensions.data(),						//ppEnabledExtensionNames
			&featuresInfo									//pEnabledFeatures
		};

		VkResult createDeviceResult = vkCreateDevice(device, &logicalDeviceInfo, nullptr, &m_logicalDevice);
		if (createDeviceResult != VK_SUCCESS)
		{
			throw std::runtime_error("Creation of logical device failed with error code: " + std::to_string(createDeviceResult));
		}

		vkGetDeviceQueue(m_logicalDevice, qfIndices.queueFamilies["graphicsQueueFamily"], 0, &m_requestedQueues["graphicsQueue"]);
		vkGetDeviceQueue(m_logicalDevice, qfIndices.queueFamilies["surfaceQueueFamily"], 0, &m_requestedQueues["surfaceQueue"]);
	}
#endif

void LogicalDeviceWrapper::DestroyLogicalDevice()
{
	if (m_logicalDevice != VK_NULL_HANDLE) { vkDestroyDevice(m_logicalDevice, nullptr); }
}