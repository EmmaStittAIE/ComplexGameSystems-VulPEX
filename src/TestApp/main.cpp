#include <iostream>
#include <stdexcept>

#include <glm.hpp>

#include "VulkanApplication.hpp"
#include "Logger.h"

int main(void)
{
    VulkanApplication vkApp;

	// TODO: better error handling. Currently we just... relay the info in the exception and end the program. Better than nothing, but not great
    try
    {
		vkApp.Init();

        vkApp.Run();
    }
    catch (const std::exception& ex)
    {
        Logger::Log( { "Application encountered an exception: ", ex.what() }, LogType::Error );
        return 1;
    }
}
