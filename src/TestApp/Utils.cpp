#include "Utils.hpp"

#include <random>

const std::string Utils::LoadFileToString(std::string filePath)
{
	std::stringstream fileContent;

	std::ifstream file;
	file.open(filePath, file.in);

	if (file.fail())
	{
		// TODO: error reporting
		return "";
	}

	while (!(file.peek() == EOF))
	{
		fileContent << (char)file.get();
	}

	return fileContent.str();
}

const std::string Utils::GetRandomString(std::vector<std::string> listOfStrings)
{
	// Some <random> magic. Essentially, setup a seeded random generator, and give it minimum (inclusive) and maximum (exclusive) values
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, listOfStrings.size());

	return listOfStrings[dist(mt)];
}
