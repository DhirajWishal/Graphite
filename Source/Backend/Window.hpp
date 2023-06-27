// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "InstanceBoundObject.hpp"

#include <string_view>
#include <vector>

struct SDL_Window;

/**
 * Window class.
 * This class contains a single window instance used by the engine and contains the event system.
 */
class Window final : public InstanceBoundObject
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 * @param title The title of the window.
	 */
	explicit Window(Instance& instance, std::string_view title);

	/**
	 * Destructor.
	 */
	~Window() override;

	/**
	 * Update the window.
	 * This will also poll for inputs.
	 */
	void update();

public:
	GRAPHITE_SETUP_SIMPLE_GETTER(uint32_t, Width, m_Width);
	GRAPHITE_SETUP_SIMPLE_GETTER(uint32_t, Height, m_Height);
	GRAPHITE_SETUP_SIMPLE_GETTER(VkSurfaceKHR, Surface, m_Surface);
	GRAPHITE_SETUP_SIMPLE_GETTER(VkSwapchainKHR, Swapchain, m_Swapchain);

private:
	/**
	 * Setup the swapchain.
	 */
	void setupSwapchain();

	/**
	 * Setup the image views for the swapchain images.
	 */
	void setupImageViews();

private:
	std::vector<VkImage> m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainImageViews;

	SDL_Window* m_pWindow = nullptr;

	uint32_t m_Width = 0;
	uint32_t m_Height = 0;

	uint32_t m_FrameCount = 0;

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

	VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;
};