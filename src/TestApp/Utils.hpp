#pragma once
#include <sstream>
#include <fstream>
#include <vector>

namespace Utils
{
	extern const std::string LoadFileToString(std::string filePath);

	extern const std::string GetRandomString(std::vector<std::string> listOfStrings);
}