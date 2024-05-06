#pragma once

#include <string>
#include <initializer_list>

enum LogType
{
	Info,
	Warning,
	Error
};

namespace Logger
{
	extern void Log(std::initializer_list<std::string> messageText, LogType type);
}