#include <stdexcept>
#include <vector>
#include <sstream>

#include <VulkanApplication.hpp>
#include <Modules/DataStructures/DefaultVertex.hpp>

#include <Logger.hpp>
#include <FileHandling.hpp>
#include <Rand.hpp>

struct MyVertex {
	Vec3 pos;
	Vec3 normal;

	MyVertex(Vec3 pos_, Vec3 normal_) { pos = pos_, normal = normal_; };

	static uint32_t GetSizeOf()
	{
		return sizeof(MyVertex);
	}

	// Change size of array if number of variables changes
	static std::array<std::pair<vk::Format, uint32_t>, 2> GetVarInfo()
	{
		std::pair<vk::Format, uint32_t> vertPosInfo = {(vk::Format)VulkanFormat::eVec3, offsetof(MyVertex, pos)};
		std::pair<vk::Format, uint32_t> vertNormInfo = {(vk::Format)VulkanFormat::eVec3, offsetof(MyVertex, normal)};
		return {vertPosInfo, vertNormInfo};
	}
};

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

	std::vector<const char*> extensions;

	// TODO: Instead of giving our application verts in setup, give it to the application during runtime
	std::vector<DataStructures::Vertex> verts = {
		{{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    	{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    	{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	/*std::vector<MyVertex> myVerts = {
		{{ 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    	{{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    	{{-0.5f,  0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}
	};*/

    vkApp.Init(winInfo, appInfo, extensions, {});

	std::array vertexInfo = DataStructures::Vertex::GetVarInfo();
	vkApp.GraphicsPipelineSetup(shaderInfo, DataStructures::Vertex::GetSizeOf(), vertexInfo.data(), vertexInfo.size(), verts);

    while (vkApp.IsRunning())
    {
		// TODO: When the player passes vertices here, ensure thet they're of the same type as DataStrcutures::Vertex
    	vkApp.RenderFrame(verts);
		
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
