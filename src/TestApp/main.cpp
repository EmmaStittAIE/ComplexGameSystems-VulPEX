#include <stdexcept>
#include <vector>
#include <sstream>

#include <VulkanApplication.hpp>
#include <Logger.hpp>
#include <FileHandling.hpp>
#include <Rand.hpp>

int entryPoint()
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

	ShaderInfo shaderInfo
	{
		FileHandling::LoadFileToByteArray("Assets/Shaders/SPIR-V/defaultVert.spv"),	//vertBytecode
		FileHandling::LoadFileToByteArray("Assets/Shaders/SPIR-V/defaultFrag.spv")	//fragBytecode
	};

	std::vector<const char *> extensions;

    vkApp.Init(winInfo, appInfo, shaderInfo, extensions, {});

    while (vkApp.IsRunning())
    {
    	vkApp.RenderFrame();
		
    	// Events (input, etc.)
    	glfwPollEvents();
    }

	// Would be nice to have this happen automatically somehow...
	vkApp.SynchroniseBeforeQuit();

	return 0;
}

int main(void)
{
	try
    {
		// In an actual application, this would be where I'd instantiate and run an application object of some kind
		return entryPoint();
    }
    catch (const std::exception& ex)
    {
		// In an actual application, it would be better to have this display in a popup somewhere (maybe a bug report dialogue), save to a log file, etc.
        Logger::Log( { "Application encountered an exception: ", ex.what() }, LogType::Fatal );
        return 1;
    }
	catch(...)
	{
		Logger::Log( { "Application encountered an unknown error." }, LogType::Fatal );
		return 1;
	}
}
