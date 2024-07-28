#include "LogicalDeviceWrapper.hpp"

#include <set>

// Private
QueueFamilyIndices LogicalDeviceWrapper::GetAvailableQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	QueueFamilyIndices indices;

	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	// TODO: this feels icky, too many strange caveats
	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.queueFamilies.insert_or_assign("graphicsQueueFamily", i);
		}

		uint32_t surfaceSupport = device.getSurfaceSupportKHR(i, surface);		

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
	m_requestedQueues = {{"graphicsQueue", nullptr}, {"surfaceQueue", nullptr}};
}

void LogicalDeviceWrapper::ConfigureLogicalDevice(std::unordered_map<std::string, vk::Queue> queues)
{
	m_requestedQueues = queues;
}

// Public
#ifdef _DEBUG
	void LogicalDeviceWrapper::CreateLogicalDevice(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char*> deviceExtensions, std::vector<const char*> validationLayers)
	{
		m_qfIndices = GetAvailableQueueFamilies(device, surface);
		if (!m_qfIndices.NecessaryFamiliesFilled())
		{
			throw std::runtime_error("Could not create logical device, required queue families not available");
		}

		std::vector<vk::DeviceQueueCreateInfo> queueInfoList;

		// This is a set because we *cannot* have duplicate queue family indices
		std::set<uint32_t> uniqueQueueFamilies = { m_qfIndices.queueFamilies["graphicsQueueFamily"], m_qfIndices.queueFamilies["surfaceQueueFamily"] };

		float queuePriority = 1;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueInfo(
				{},												//flags
				queueFamilyIndex,								//queueFamilyIndex
				1,												//queueCount
				&queuePriority									//pQueuePriorities
			);

			queueInfoList.push_back(queueInfo);
		}

		vk::PhysicalDeviceFeatures featuresInfo{};

		// Validation layers
		// Vulkan no longer makes a distinction between instance-level and device-level validation layers
		// However, since the user could be using an older version of Vulkan, we still define them so as to be compatible
		uint32_t enabledLayerCount = 0;
		const char* const* enabledLayerNames = nullptr;

		enabledLayerCount = validationLayers.size();
		enabledLayerNames = validationLayers.data();
		
		vk::DeviceCreateInfo logicalDeviceInfo(
			{},										//flags
			(uint32_t)queueInfoList.size(),			//queueCreateInfoCount
			queueInfoList.data(),					//pQueueCreateInfos
			enabledLayerCount,						//enabledLayerCount
			enabledLayerNames,						//ppEnabledLayerNames
			(uint32_t)deviceExtensions.size(),		//enabledExtensionCount
			deviceExtensions.data(),				//ppEnabledExtensionNames
			&featuresInfo							//pEnabledFeatures
		);

		m_logicalDevice = device.createDevice(logicalDeviceInfo);

		m_requestedQueues["graphicsQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies["graphicsQueueFamily"], 0);
		m_requestedQueues["surfaceQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies["surfaceQueueFamily"], 0);
	}
#else
	void LogicalDeviceWrapper::CreateLogicalDevice(vk::PhysicalDevice device, vk::SurfaceKHR surface, std::vector<const char*> deviceExtensions)
	{
		QueueFamilyIndices m_qfIndices = GetAvailableQueueFamilies(device, surface);
		if (!m_qfIndices.NecessaryFamiliesFilled())
		{
			throw std::runtime_error("Could not create logical device, required queue families not available");
		}

		std::vector<vk::DeviceQueueCreateInfo> queueInfoList;

		// This is a set because we *cannot* have duplicate queue family indices
		std::set<uint32_t> uniqueQueueFamilies = { m_qfIndices.queueFamilies["graphicsQueueFamily"], m_qfIndices.queueFamilies["surfaceQueueFamily"] };

		float queuePriority = 1;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueInfo(
				{},												//flags
				queueFamilyIndex,								//queueFamilyIndex
				1,												//queueCount
				&queuePriority									//pQueuePriorities
			);

			queueInfoList.push_back(queueInfo);
		}

		vk::PhysicalDeviceFeatures featuresInfo{};

		vk::DeviceCreateInfo logicalDeviceInfo(
			{},												//flags
			(uint32_t)queueInfoList.size(),					//queueCreateInfoCount
			queueInfoList.data(),							//pQueueCreateInfos
			0,												//enabledLayerCount
			nullptr,										//ppEnabledLayerNames
			(uint32_t)deviceExtensions.size(),				//enabledExtensionCount
			deviceExtensions.data(),						//ppEnabledExtensionNames
			&featuresInfo									//pEnabledFeatures
		);

		m_logicalDevice = device.createDevice(logicalDeviceInfo);

		m_requestedQueues["graphicsQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies["graphicsQueueFamily"], 0);
		m_requestedQueues["surfaceQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies["surfaceQueueFamily"], 0);
	}
#endif

void LogicalDeviceWrapper::DestroyLogicalDevice()
{
	if (m_logicalDevice != nullptr) { m_logicalDevice.destroy(); }
}