#include <stdexcept>

#include <VulkanApplication.hpp>

#include "Logger.hpp"

int main(void)
{
    VulkanApplication vkApp;

	WindowInfo winInfo
	{
		nullptr,
		"Don't forget to randomise this!!!",
		800,
		600
	};

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

	std::vector<const char *> extensions;

	// TODO: better error handling. Currently we just... relay the info in the exception and end the program. Better than nothing, but not great
    try
    {
		vkApp.Init(winInfo, appInfo, extensions, 0);

        vkApp.Run();
    }
    catch (const std::exception& ex)
    {
        Logger::Log( { "Application encountered an exception: ", ex.what() }, LogType::Error );
        return 1;
    }
}
