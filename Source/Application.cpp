// Copyright (c) 2023 Dhiraj Wishal

#include "Application.hpp"

#include "Core/Logging.hpp"

#include <optick.h>

Application::Application()
	: m_Window(m_Instance, "Graphite Engine")
{
}

Application::~Application()
{
}

int Application::execute()
{
	// Main iteration loop.
	while (m_bShoudRun)
	{
		OPTICK_FRAME("Main loop");
		m_Window.update();
	}

	return m_ExitCode;
}
