#include "VulkanApplication.hpp"

#include <vector>

// Private Methods

void VulkanApplication::Update(float delta)
{

}

void VulkanApplication::Render()
{

}

// Public Methods

VulkanApplication::VulkanApplication()
{
    // --Init GLFW--
    if(!glfwInit()) { throw std::runtime_error("GLFW failed to initialise"); }

	// Disable OpenGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);    
}

void VulkanApplication::Init(WindowInfo winInfo, VkApplicationInfo appInfo, std::vector<const char*> vkExtensions, VkInstanceCreateFlags vkFlags)
{
    // Create window
    m_window = glfwCreateWindow(winInfo.width, winInfo.height, winInfo.title, winInfo.targetMonitor, nullptr);

	glfwGetWindowSize(m_window, &m_winDimensions.x, &m_winDimensions.y);

    // --Init Vulkan--
	// Get Extension Info

	// Retrieve glfw's list of required extensions
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add reqired extensions to vkExtensions
	for (uint i = 0; i < glfwExtensionCount; i++)
	{
		// If the user hasn't already, add the required extensions to our extension list
		if (std::find(vkExtensions.begin(), vkExtensions.end(), glfwExtensions[i]) == vkExtensions.end())
		{
			vkExtensions.push_back(glfwExtensions[i]);
		}
	}

	// Get extension compatibility info

	// First, find out how many supported extensions there are
	uint32_t supportedExtensionsCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, nullptr);

	// Then, create an array to store the actual info about those extensions
	std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);

	// Finally, retrieve the info on all supported extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, supportedExtensions.data());

	// Then (in a very bad and ugly fashion), check if these extensions are supported by the system
	// TODO: Make this... less... bad?
	std::vector<const char*> unsupportedExtensions;
	for (uint i = 0; i < vkExtensions.size(); i++)
	{
		bool extensionSupported = false;

		for (uint j = 0; j < supportedExtensionsCount; j++)
		{
			if (strcmp(supportedExtensions[j].extensionName, vkExtensions[i]) == 0)
			{
				extensionSupported = true;
				break;
			}
		}
		
		if (!extensionSupported)
		{
			throw std::runtime_error("One or more extensions required by GLFW are not supported by the target system"); 
		}
	}

	// Configure Instance Info
	VkInstanceCreateInfo instanceInfo
	{
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,	//sType
		NULL,									//pNext
		vkFlags,								//flags
		&appInfo,								//pApplicationInfo
		0,										//enabledLayerCount
		NULL,									//ppEnabledLayerNames
		(uint32_t)vkExtensions.size(),			//enabledExtensionCount
		vkExtensions.data()						//ppEnabledExtensionNames
	};

	// Create info, custom memory allocator callback, pointer to instance
	VkResult result = vkCreateInstance(&instanceInfo, nullptr, &vulkanInstance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Creation of Vulkan instance failed with error code: " + std::to_string(result));
	}
}

void VulkanApplication::Run()
{
    while (IsRunning())
    {
        Render();

        // Events (input, etc.)
        glfwPollEvents();
    }
}

bool VulkanApplication::IsRunning()
{
    return !glfwWindowShouldClose(m_window);
}

VulkanApplication::~VulkanApplication()
{
	vkDestroyInstance(vulkanInstance, nullptr);

	if (m_window != nullptr) { glfwDestroyWindow(m_window); }

    glfwTerminate();
}