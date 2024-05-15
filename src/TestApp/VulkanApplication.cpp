#include "VulkanApplication.hpp"

#include <vector>

#include "Logger.h"

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

void VulkanApplication::Init()
{
    // Create window
    m_window = glfwCreateWindow(m_res.width, m_res.height, "Don't forget to randomise this!!!", nullptr, nullptr);

    // --Init Vulkan--
    // Configure App Info
    VkApplicationInfo appInfo
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,		//sType
        NULL,									//pNext
        "Test App",								//pApplicationName
        VK_MAKE_VERSION(0, 0, 1),				//applicationVersion
		NULL,									//pEngineName
		0,										//engineVersion
		VK_API_VERSION_1_3						//apiVersion
    };

	// Get Extension Info
	
	// First, find out how many supported extensions there are
	uint32_t supportedExtensionsCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, nullptr);

	// Then, create an array to store the actual info about those extensions
	std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionsCount);

	// Finally, retrieve the info on all supported extensions
	vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionsCount, supportedExtensions.data());

	// Retrieve glfw's list of required extensions
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Then (in a very bad and ugly fashion), check if these extensions are supported by the system
	// TODO: Make this... less... bad?
	std::vector<const char*> unsupportedExtensions;
	for (int i = 0; i < glfwExtensionCount; i++)
	{
		bool extensionSupported = false;

		for (int j = 0; j < supportedExtensionsCount; j++)
		{
			if (strcmp(supportedExtensions[j].extensionName, glfwExtensions[i]) == 0)
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
		0,										//flags
		&appInfo,								//pApplicationInfo
		0,										//enabledLayerCount
		NULL,									//ppEnabledLayerNames
		glfwExtensionCount,						//enabledExtensionCount
		glfwExtensions							//ppEnabledExtensionNames
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