// Copyright (c) 2023 Dhiraj Wishal

#include "Window.hpp"
#include "Instance.hpp"

#include "Core/Logging.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <optick.h>

namespace /* anonymous */
{
	/**
	 * Static initializer structure.
	 * This structure can be used to initialize something ONCE and destroy when closing the application.
	 */
	struct StaticInitializer final
	{
		/**
		 * Default constructor.
		 */
		StaticInitializer()
		{
			// Try and initialize SDL.
			if (SDL_Init(SDL_INIT_VIDEO) != 0)
				GRAPHITE_LOG_FATAL("Failed to initialize SDL! {}", SDL_GetError());

			else
				GRAPHITE_LOG_INFORMATION("Successfully initialized SDL.");
		}

		/**
		 * Destructor.
		 */
		~StaticInitializer()
		{
			// Unload the Vulkan library and quit SDL.
			SDL_Vulkan_UnloadLibrary();
			SDL_Quit();
		}
	};
}

// Initialize SDL.
static StaticInitializer g_Initializer;

Window::Window(Instance& instance, std::string_view title)
	: InstanceBoundObject(instance)
{
	// Create the window.
	m_pWindow = SDL_CreateWindow(title.data(), 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN);

	// Check if the window was created properly.
	if (!m_pWindow)
	{
		GRAPHITE_LOG_FATAL("Failed to create the window!");
		return;
	}

	// Set this class as user data.
	SDL_SetWindowData(m_pWindow, "this", this);

	// Make sure to show the window.
	// SDL_ShowWindow(m_pWindow);

	// Create the window surface.
	if (SDL_Vulkan_CreateSurface(m_pWindow, m_Instance.getInstance(), &m_Surface) == SDL_FALSE)
	{
		GRAPHITE_LOG_FATAL("Failed to create the Vulkan surface!");
	}
}

void Window::update()
{
	OPTICK_EVENT();

	SDL_Event events;
	SDL_PollEvent(&events);
}
