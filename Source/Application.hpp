// Copyright (c) 2023 Dhiraj Wishal

#pragma once

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
	bool m_bShoudRun = true;
};