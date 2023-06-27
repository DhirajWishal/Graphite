// Copyright (c) 2023 Dhiraj Wishal

#include "Window.hpp"
#include "Instance.hpp"
#include "VulkanMacros.hpp"

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

	// Create the window surface.
	if (SDL_Vulkan_CreateSurface(m_pWindow, m_Instance.getInstance(), &m_Surface) == SDL_FALSE)
		GRAPHITE_LOG_FATAL("Failed to create the Vulkan surface!");

	// If we created the surface, setup the swapchain.
	else
		setupSwapchain();
}

Window::~Window()
{
	m_Instance.waitIdle();

	m_Instance.getLogicalDevice().access([this](VkDevice logicalDevice)
		{
			for (const auto view : m_SwapchainImageViews)
				m_Instance.getDeviceTable().vkDestroyImageView(logicalDevice, view, nullptr);

			m_Instance.getDeviceTable().vkDestroySwapchainKHR(logicalDevice, m_Swapchain, nullptr);
		}
	);

	vkDestroySurfaceKHR(m_Instance.getInstance(), m_Surface, nullptr);

	m_SwapchainImageViews.clear();
	m_Swapchain = VK_NULL_HANDLE;
}

void Window::update()
{
	OPTICK_EVENT();

	SDL_Event events;
	while (SDL_PollEvent(&events))
	{
		// Handle the events.
	}
}

void Window::setupSwapchain()
{
	// Get the surface capabilities.
	const auto surfaceCapabilities = m_Instance.getPhysicalDevice().access([this](VkPhysicalDevice physicalDevice)
		{
			VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
			GRAPHITE_VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilities), "Failed to get the surface capabilities!");
			return surfaceCapabilities;
		}
	);

	// Resolve the surface composite.
	auto surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(surfaceCapabilities.supportedCompositeAlpha);
	if (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		surfaceComposite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	else if (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
		surfaceComposite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

	else if (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
		surfaceComposite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
	else
		surfaceComposite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

	m_FrameCount = std::clamp(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
	m_Width = surfaceCapabilities.currentExtent.width;
	m_Height = surfaceCapabilities.currentExtent.height;

	// Get the present modes.
	const auto presentModes = m_Instance.getPhysicalDevice().access([this](VkPhysicalDevice physicalDevice) -> std::vector<VkPresentModeKHR>
		{
			uint32_t presentModeCount = 0;
			GRAPHITE_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr), "Failed to get the surface present mode count!");

			if (presentModeCount == 0)
			{
				GRAPHITE_LOG_FATAL("No suitable present formats found!");
				return {};
			}

			std::vector<VkPresentModeKHR> presentModes(presentModeCount);
			GRAPHITE_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data()), "Failed to get the surface present modes!");

			return presentModes;
		}
	);

	// Check if we have the present mode we need.
	VkPresentModeKHR presentMode = presentModes.front();
	for (const auto availablePresentMode : presentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
	}

	// Get the surface formats.
	const auto surfaceFormats = m_Instance.getPhysicalDevice().access([this](VkPhysicalDevice physicalDevice) -> std::vector<VkSurfaceFormatKHR>
		{
			uint32_t formatCount = 0;
			GRAPHITE_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr), "Failed to get the surface format count!");

			if (formatCount == 0)
			{
				GRAPHITE_LOG_FATAL("No suitable surface formats found!");
				return {};
			}

			std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
			GRAPHITE_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data()), "Failed to get the surface formats!");

			return surfaceFormats;
		}
	);

	// Get the best surface format.
	VkSurfaceFormatKHR surfaceFormat = surfaceFormats.front();
	for (const auto& availableFormat : surfaceFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			surfaceFormat = availableFormat;
			break;
		}
	}

	m_SwapchainFormat = surfaceFormat.format;

	// Create the swapchain.
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = m_Surface;
	createInfo.minImageCount = m_FrameCount;
	createInfo.imageFormat = m_SwapchainFormat;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = surfaceCapabilities.currentExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = surfaceCapabilities.currentTransform;
	createInfo.compositeAlpha = surfaceComposite;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// Resolve the queue families if the two queues are different.
	const std::array<uint32_t, 2> queueFamilyindices = {
		m_Instance.getGraphicsQueue().getUnsafe().m_Family,
		m_Instance.getTransferQueue().getUnsafe().m_Family
	};

	if (queueFamilyindices[0] != queueFamilyindices[1])
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = queueFamilyindices.size();
		createInfo.pQueueFamilyIndices = queueFamilyindices.data();
	}

	// Create the swapchain.
	m_Instance.getLogicalDevice().access([this, &createInfo](VkDevice logicalDevice)
		{
			GRAPHITE_VK_ASSERT(m_Instance.getDeviceTable().vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &m_Swapchain), "Failed to create the swapchain!");
		}
	);

	// Get the swapchain images.
	m_SwapchainImages.resize(m_FrameCount);
	m_Instance.getLogicalDevice().access([this](VkDevice logicalDevice)
		{
			GRAPHITE_VK_ASSERT(m_Instance.getDeviceTable().vkGetSwapchainImagesKHR(logicalDevice, m_Swapchain, &m_FrameCount, m_SwapchainImages.data()), "Failed to get the swapchain images!");
		}
	);

	// Setup the image views.
	setupImageViews();
}

void Window::setupImageViews()
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = VK_NULL_HANDLE;
	createInfo.flags = 0;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = m_SwapchainFormat;
	createInfo.components = {};
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	m_SwapchainImageViews.resize(m_SwapchainImages.size());

	// Iterate over the image views and create them.
	VkImageView* pArray = m_SwapchainImageViews.data();
	for (auto itr = m_SwapchainImages.begin(); itr != m_SwapchainImages.end(); ++itr, ++pArray)
	{
		createInfo.image = *itr;

		m_Instance.getLogicalDevice().access([this, &createInfo, pArray](VkDevice logicalDevice)
			{
				GRAPHITE_VK_ASSERT(m_Instance.getDeviceTable().vkCreateImageView(logicalDevice, &createInfo, nullptr, pArray), "Failed to create the swapchain image view!");
			}
		);
	}
}
