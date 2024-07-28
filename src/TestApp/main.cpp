#include <stdexcept>
#include <vector>
#include <sstream>

#include <VulkanApplication.hpp>
#include <Logger.hpp>
#include <FileHandling.hpp>
#include <Rand.hpp>

int main(void)
{
	std::map<int, int> windowHints = {{GLFW_RESIZABLE, GLFW_FALSE}};

	// Create our VulkanApplication
    VulkanApplication vkApp(windowHints);

	// Generate random window title
	std::stringstream titleStringsStream(FileHandling::LoadFileToString("Assets/TitleStrings.txt"));

	std::vector<std::string> titleStrings;
	std::string substring;
	char deliminator = '\n';
	while (std::getline(titleStringsStream, substring, deliminator))
	{
		titleStrings.push_back(substring);
	}

	std::string windowTitle = Rand::GetRandomStringFromList(titleStrings);

	// Configure window info
	WindowInfo winInfo
	{
		nullptr,				//targetMonitor
		windowTitle.c_str(),	//title
		800,					//width
		600						//height
	};

	// Configure App Info
    vk::ApplicationInfo appInfo(
		"Test App",								//pApplicationName
        VK_MAKE_VERSION(0, 0, 1),				//applicationVersion
		nullptr,								//pEngineName
		0,										//engineVersion
		VK_API_VERSION_1_3						//apiVersion
	);

	std::vector<const char *> extensions;

    try
    {
		vkApp.Init(winInfo, appInfo, extensions, {});

        while (vkApp.IsRunning())
    	{
        	//Render();

        	// Events (input, etc.)
        	glfwPollEvents();
    	}
    }
    catch (const std::exception& ex)
    {
		// In an actual application, it would be better to have this display in a popup somewhere (maybe a bug report dialogue), save to a log file, etc.
        Logger::Log( { "Application encountered an exception: ", ex.what() }, LogType::Error );
        return 1;
    }
}
