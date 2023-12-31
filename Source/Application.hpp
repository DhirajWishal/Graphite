// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "Backend/Instance.hpp"
#include "Backend/Window.hpp"

/**
 * Application class.
 * This is the main application instance and contains all the frontend and backend systems.
 */
class Application final
{
public:
	/**
	 * Default constructor.
	 */
	Application();

	/**
	 * Destructor.
	 */
	~Application();

	/**
	 * Execute function.
	 * This contains the main loop.
	 *
	 * @return The exit code.
	 */
	int execute();

private:
	Instance m_Instance;
	Window m_Window;

	int m_ExitCode = 0;
	bool m_bShoudRun = true;
};