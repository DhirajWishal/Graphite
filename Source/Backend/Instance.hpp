// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "Core/Guarded.hpp"

#if defined(GRAPHITE_PLATFORM_WINDOWS)
#	define VK_USE_PLATFORM_WIN32_KHR

#elif defined(GRAPHITE_PLATFORM_MAC)
#	define VK_USE_PLATFORM_MACOS_MVK

#elif defined(GRAPHITE_PLATFORM_LINUX_X11)
#	define VK_USE_PLATFORM_XLIB_KHR
#	define VK_USE_PLATFORM_XCB_KHR

#elif defined(GRAPHITE_PLATFORM_LINUX_WAYLAND)
#	define VK_USE_PLATFORM_WAYLAND_KHR

#else 
#	error Unsupported platform!

#endif

#include <volk.h>
#include <vk_mem_alloc.h>

#include <vector>
#include <array>
#include <fstream>

/**
 * Vulkan queue structure.
 * This contains the Vulkan queue handle and it's family.
 */
struct VulkanQueue final
{
	VkQueue m_Queue = VK_NULL_HANDLE;
	uint32_t m_Family = 0;
};

/**
 * Instance class.
 * This contains the main Vulkan instance which is used by the engine.
 */
class Instance final
{
public:
	/**
	 * Default constructor.
	 */
	Instance();

	/**
	 * Destructor.
	 */
	~Instance();

	/**
	 * Get the Vulkan log file.
	 * This is file is per-instance and is generated at runtime.
	 *
	 * @return The log file.
	 */
	[[nodiscard]] std::ofstream& getLogFile() { return m_LogFile; }

public:
	/**
	 * Get the graphics queue object.
	 *
	 * @return The guarded queue.
	 */
	[[nodiscard]] Guarded<VulkanQueue>& getGraphicsQueue() { return m_Queues[0]; }

	/**
	 * Get the graphics queue object.
	 *
	 * @return The guarded queue.
	 */
	[[nodiscard]] const Guarded<VulkanQueue>& getGraphicsQueue() const { return m_Queues[0]; }

	/**
	 * Get the compute queue object.
	 *
	 * @return The guarded queue.
	 */
	[[nodiscard]] Guarded<VulkanQueue>& getComputeQueue() { return m_Queues[1]; }

	/**
	 * Get the compute queue object.
	 *
	 * @return The guarded queue.
	 */
	[[nodiscard]] const Guarded<VulkanQueue>& getComputeQueue() const { return m_Queues[1]; }

	/**
	 * Get the transfer queue object.
	 *
	 * @return The guarded queue.
	 */
	[[nodiscard]] Guarded<VulkanQueue>& getTransferQueue() { return m_Queues[2]; }

	/**
	 * Get the transfer queue object.
	 *
	 * @return The guarded queue.
	 */
	[[nodiscard]] const Guarded<VulkanQueue>& getTransferQueue() const { return m_Queues[2]; }

private:
	/**
	 * Create the instance object.
	 */
	void createInstance();

	/**
	 * Select the best physical device.
	 */
	void selectPhysicalDevice();

	/**
	 * Create the logical device.
	 */
	void createLogicalDevice();

	/**
	 * Create the Vulkan memory allocator.
	 */
	void createMemoryAllocator();

private:
	VkPhysicalDeviceProperties m_PhysicalDeviceProperties;

	std::ofstream m_LogFile;

	std::array<Guarded<VulkanQueue>, 3> m_Queues;

	VolkDeviceTable m_DeviceTable;

	VkInstance m_Instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_LogicalDevice = VK_NULL_HANDLE;

	Guarded<VmaAllocator> m_Allocator = nullptr;

	std::vector<const char*> m_ValidationLayers;
	std::vector<const char*> m_DeviceExtensions;
};