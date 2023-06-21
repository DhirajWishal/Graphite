// Copyright (c) 2023 Dhiraj Wishal

#include "Application.hpp"

#include <spdlog/spdlog.h>

Application::Application()
{
	spdlog::info("Welcome to Graphite!");
}

int Application::execute()
{
	// Main iteration loop.
	while (m_bShoudRun)
	{

	}

	return 0;
}
