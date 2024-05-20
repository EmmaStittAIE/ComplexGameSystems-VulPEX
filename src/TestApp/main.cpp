#include <stdexcept>
#include <vector>
#include <sstream>

#include <VulkanApplication.hpp>

#include "Logger.hpp"
#include "Utils.hpp"

int main(void)
{
	// Create our VulkanApplication
    VulkanApplication vkApp;

	// Generate random window title
	std::stringstream titleStringsStream(Utils::LoadFileToString("Assets/TitleStrings.txt"));

	std::vector<std::string> titleStrings;
	std::string substring;
	char deliminator = '\n';
	while (std::getline(titleStringsStream, substring, deliminator))
	{
		titleStrings.push_back(substring);
	}

	std::string windowTitle = Utils::GetRandomString(titleStrings);

	// Configure window info
	WindowInfo winInfo
	{
		nullptr,				//targetMonitor
		windowTitle.c_str(),	//title
		800,					//width
		600						//height
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

        while (vkApp.IsRunning())
    	{
        	//Render();

        	// Events (input, etc.)
        	glfwPollEvents();
    	}
    }
    catch (const std::exception& ex)
    {
        Logger::Log( { "Application encountered an exception: ", ex.what() }, LogType::Error );
        return 1;
    }
}
