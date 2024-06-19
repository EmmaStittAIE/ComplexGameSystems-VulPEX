#include "WindowWrapper.hpp"

WindowWrapper::WindowWrapper(std::map<int, int> windowHints)
{
	// --Init GLFW--
    if(!glfwInit()) { throw std::runtime_error("GLFW failed to initialise"); }

	// Disable OpenGL
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	for (auto& [hint, value] : windowHints)
	{
		glfwWindowHint(hint, value);
	}
}

void WindowWrapper::CreateWindow(WindowInfo winInfo)
{
	m_window = glfwCreateWindow(winInfo.width, winInfo.height, winInfo.title, winInfo.targetMonitor, nullptr);

	glfwGetWindowSize(m_window, &m_winDimensions.x, &m_winDimensions.y);
}

void WindowWrapper::DestroyWindow()
{
	if (m_window != nullptr) { glfwDestroyWindow(m_window); }
}
