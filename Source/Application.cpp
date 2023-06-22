// Copyright (c) 2023 Dhiraj Wishal

#include "Application.hpp"

#include "Core/Logging.hpp"

#include <SDL3/SDL.h>
#include <optick.h>

Application::Application()
{
	spdlog::info("Welcome to Graphite!");

	// Try and initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		GRAPHITE_LOG_FATAL("Failed to initialize SDL!");
		m_ExitCode = -1;
		return;
	}

	GRAPHITE_LOG_INFORMATION("SDL initialized successfully.");
}

Application::~Application()
{
	// Quit SDL.
	SDL_Quit();
}

int Application::execute()
{
	// Main iteration loop.
	while (m_bShoudRun)
	{
		OPTICK_FRAME("Main loop");
	}

	return m_ExitCode;
}
