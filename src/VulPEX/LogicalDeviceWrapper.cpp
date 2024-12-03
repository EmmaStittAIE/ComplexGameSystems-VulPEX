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

		// For our transfer family, we explicitly want families that support transfer, but not graphics
		// Otherwise, we may as well fall back on graphics
		if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) &&
			!(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics))
		{
			indices.queueFamilies.insert_or_assign("transferQueueFamily", i);
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
	m_queues = {{"graphicsQueue", nullptr}, {"surfaceQueue", nullptr}, {"transferQueue", nullptr}};
}

void LogicalDeviceWrapper::ConfigureLogicalDevice(std::vector<std::string> requestedQueueFamilies)
{
	for (std::string queueFamily : requestedQueueFamilies)
	{
		m_queues.insert_or_assign(queueFamily, nullptr);
	}
	
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
		std::set<uint32_t> uniqueQueueFamilies = { m_qfIndices.queueFamilies.at("graphicsQueueFamily"), m_qfIndices.queueFamilies.at("surfaceQueueFamily"),
												   m_qfIndices.queueFamilies.at("transferQueueFamily") };

		float queuePriority = 1;
		for (uint32_t queueFamilyIndex : uniqueQueueFamilies)
		{
			vk::DeviceQueueCreateInfo queueInfo(
				{},					//flags
				queueFamilyIndex,	//queueFamilyIndex
				1,					//queueCount
				&queuePriority		//pQueuePriorities
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
			{},									//flags
			(uint32_t)queueInfoList.size(),		//queueCreateInfoCount
			queueInfoList.data(),				//pQueueCreateInfos
			enabledLayerCount,					//enabledLayerCount
			enabledLayerNames,					//ppEnabledLayerNames
			(uint32_t)deviceExtensions.size(),	//enabledExtensionCount
			deviceExtensions.data(),			//ppEnabledExtensionNames
			&featuresInfo						//pEnabledFeatures
		);

		m_logicalDevice = device.createDevice(logicalDeviceInfo);

		// m_queues is set up to contain only the queues that the user needs, so we won't fill ones that aren't already there
		if (m_queues.contains("graphicsQueue")) { m_queues["graphicsQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies.at("graphicsQueueFamily"), 0); }
		if (m_queues.contains("surfaceQueue")) { m_queues["surfaceQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies.at("surfaceQueueFamily"), 0); }
		if (m_queues.contains("transferQueue")) { m_queues["transferQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies.at("transferQueueFamily"), 0); }
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
		std::set<uint32_t> uniqueQueueFamilies = { m_qfIndices.queueFamilies.at("graphicsQueueFamily"), m_qfIndices.queueFamilies.at("surfaceQueueFamily"),
												   m_qfIndices.queueFamilies.at("transferQueueFamily") };

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

		// m_queues is set up to contain only the queues that the user needs, so we won't fill ones that aren't already there
		if (m_queues.contains("graphicsQueue")) { m_queues["graphicsQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies.at("graphicsQueueFamily"), 0); }
		if (m_queues.contains("surfaceQueue")) { m_queues["surfaceQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies.at("surfaceQueueFamily"), 0); }
		if (m_queues.contains("transferQueue")) { m_queues["transferQueue"] = m_logicalDevice.getQueue(m_qfIndices.queueFamilies.at("transferQueueFamily"), 0); }
	}
#endif

void LogicalDeviceWrapper::DestroyLogicalDevice()
{
	if (m_logicalDevice != nullptr) { m_logicalDevice.destroy(); }
}