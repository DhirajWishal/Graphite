// Copyright (c) 2023 Dhiraj Wishal

#include "Instance.hpp"
#include "VulkanMacros.hpp"

#include "Core/Common.hpp"

#include <SDL3/SDL_vulkan.h>

#include <optick.h>

#include <set>
#include <string_view>
#include <bit>

namespace /* anonymous */
{
	/**
	 * Get the required instance extensions.
	 *
	 * @return The required instance extensions.
	 */
	[[nodiscard]] std::vector<const char*> GetRequiredInstanceExtensions()
	{
		// Get the extensions.
		unsigned int count = 0;
		if (SDL_Vulkan_GetInstanceExtensions(&count, nullptr) == SDL_FALSE)
		{
			GRAPHITE_LOG_FATAL("Failed to get the instance extension count from SDL!");
			return {};
		}

		std::vector<const char*> extensions(count);
		if (SDL_Vulkan_GetInstanceExtensions(&count, extensions.data()) == SDL_FALSE)
		{
			GRAPHITE_LOG_FATAL("Failed to get the instance extensions from SDL!");
			return {};
		}

#ifdef GRAPHITE_DEBUG
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#endif

		return extensions;
	}

	/**
	 * Vulkan debug callback.
	 * This function is used by Vulkan to report any internal message to the user.
	 *
	 * @param messageSeverity The severity of the message.
	 * @param messageType The type of the message.
	 * @param pCallbackData The data passed by the API.
	 * @param The user data submitted to Vulkan before this call.
	 * @return The status return.
	 */
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		// Log to the console if we have an error or a warning.
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			GRAPHITE_LOG_ERROR("Vulkan Validation Layer: {}", pCallbackData->pMessage);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			GRAPHITE_LOG_WARNING("Vulkan Validation Layer: {}", pCallbackData->pMessage);
		}

		// Else log to the file.
		else
		{
			auto& logFile = GRAPHITE_BIT_CAST(Instance*, pUserData)->getLogFile();

			// Log if the log file is open.
			if (logFile.is_open())
			{
				logFile << "Vulkan Validation Layer: ";

				if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
					logFile << "GENERAL | ";

				else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
					logFile << "VALIDATION | ";

				else if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
					logFile << "PERFORMANCE | ";

				logFile << pCallbackData->pMessage << std::endl;

				logFile.flush();
			}
		}

		return VK_FALSE;
	}

	/**
	 * Check if the physical device supports the required queues.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param flag The queue flag to check.
	 * @return Whether or not the queues are supported.
	 */
	[[nodiscard]] bool CheckQueueSupport(VkPhysicalDevice physicalDevice, VkQueueFlagBits flag)
	{
		// Get the queue family count.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Validate if we have queue families.
		if (queueFamilyCount == 0)
		{
			GRAPHITE_LOG_FATAL("Failed to get the queue family property count!");
			return false;
		}

		// Get the queue family properties.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

		// Iterate over those queue family properties and check if we have a family with the required flag.
		for (const auto& family : queueFamilies)
		{
			if (family.queueCount == 0)
				continue;

			// Check if the queue flag contains what we want.
			if (family.queueFlags & flag)
				return true;
		}

		return false;
	}

	/**
	 * Check device extension support.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param deviceExtensions The extension to check.
	 * @return True if the device supports at least one of the device extensions.
	 * @return False if the device does not support any of the required extensions.
	 */
	[[nodiscard]] bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions)
	{
		// If there are no extension to check, we can just return true.
		if (deviceExtensions.empty())
			return true;

		// Get the extension count.
		uint32_t extensionCount = 0;
		GRAPHITE_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr), "Failed to enumerate physical device extension property count!");

		// Load the extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		GRAPHITE_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()), "Failed to enumerate physical device extension properties!");

		std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Iterate and check if it contains the extensions we need. If it does, remove them from the set so we can later check if 
		// all the required extensions exist.
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// If the required extension count did not change, we don't support any of those required extensions.
		if (requiredExtensions.size() == deviceExtensions.size())
		{
			GRAPHITE_LOG_INFORMATION("The physical device {} does not support any of the required extensions.", fmt::ptr(physicalDevice));
			return false;
		}

		// If the extension count is more than 0, that means it supports a few of those required extensions.
		if (!requiredExtensions.empty())
			GRAPHITE_LOG_INFORMATION("The physical device {} supports only some of the required extensions.", fmt::ptr(physicalDevice));

		return true;
	}

	/**
	 * Get the unsupported device extension support.
	 *
	 * @param physicalDevice The physical device to check.
	 * @param deviceExtensions The extension to check.
	 * @return True if the device supports at least one of the device extensions.
	 * @return False if the device does not support any of the required extensions.
	 */
	[[nodiscard]] std::set<std::string_view> GetUnsupportedDeviceExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions)
	{
		// If there are no extension to check, we can just return true.
		if (deviceExtensions.empty())
			return {};

		// Get the extension count.
		uint32_t extensionCount = 0;
		GRAPHITE_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr), "Failed to enumerate physical device extension property count!");

		// Load the extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		GRAPHITE_VK_ASSERT(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data()), "Failed to enumerate physical device extension properties!");

		std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Iterate and check if it contains the extensions we need. If it does, remove them from the set so we can later check if 
		// all the required extensions exist.
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// If the required extension count did not change, we don't support any of those required extensions.
		if (requiredExtensions.size() == deviceExtensions.size())
			return requiredExtensions;

		return requiredExtensions;
	}

	/**
	 * Find the physical device queue family with the required flag.
	 *
	 * @param physicalDevice The Vulkan physical device.
	 * @param flag The flag to check and get.
	 * @return The family index. -1 is returned if not found.
	 */
	[[nodiscard]] uint32_t FindPhysialDeviceQueueFamily(const VkPhysicalDevice& physicalDevice, VkQueueFlagBits flag)
	{
		// Get the queue family count.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		// Validate if we have queue families.
		if (queueFamilyCount == 0)
		{
			GRAPHITE_LOG_FATAL("Failed to get the queue family property count!");
			return false;
		}

		// Get the queue family properties.
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
		{
			if (queueFamilyProperties[i].queueFlags & flag)
				return i;
		}

		return -1;
	}
}

Instance::Instance()
{
	// Load the Vulkan library to SDL.
	if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
	{
		GRAPHITE_LOG_FATAL("Failed to load the Vulkan library in SDL! {}", SDL_GetError());
		return;
	}

	// Set up the device extensions.
	m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	m_DeviceExtensions.emplace_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

	// Initialize Volk.
	volkInitializeCustom(GRAPHITE_BIT_CAST(PFN_vkGetInstanceProcAddr, SDL_Vulkan_GetVkGetInstanceProcAddr()));

	// Create the instance.
	createInstance();

	// Select the best physical device.
	selectPhysicalDevice();

	// Create the logical device.
	createLogicalDevice();

	// Create the memory allocator.
	createMemoryAllocator();
}

Instance::~Instance()
{
	m_DeviceTable.vkDestroyDevice(m_LogicalDevice.getUnsafe(), nullptr);

#ifdef GRAPHITE_DEBUG
	const auto vkDestroyDebugUtilsMessengerEXT = GRAPHITE_BIT_CAST(PFN_vkDestroyDebugUtilsMessengerEXT, vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
	vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

#endif // GRAPHITE_DEBUG

	vkDestroyInstance(m_Instance, nullptr);
}

void Instance::waitIdle()
{
	OPTICK_EVENT();

	m_LogicalDevice.access([this](VkDevice logicalDevice)
		{
			m_DeviceTable.vkDeviceWaitIdle(logicalDevice);
		}
	);
}

void Instance::createInstance()
{
	VkApplicationInfo applicationInfo = {};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = "Graphite";
	applicationInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	applicationInfo.pEngineName = "Graphite Engine";
	applicationInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0);
	applicationInfo.apiVersion = volkGetInstanceVersion();

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.pApplicationInfo = &applicationInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;

	const auto requiredExtensions = GetRequiredInstanceExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

#ifdef GRAPHITE_DEBUG
	// Emplace the required validation layer(s).
	m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
	// m_ValidationLayers.emplace_back("VK_LAYER_LUNARG_api_dump");

	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
	debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugUtilsMessengerCreateInfo.pNext = VK_NULL_HANDLE;
	debugUtilsMessengerCreateInfo.pUserData = this;
	debugUtilsMessengerCreateInfo.flags = 0;
	debugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
	debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	// Submit it to the instance.
	createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
	createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
	createInfo.pNext = &debugUtilsMessengerCreateInfo;

#endif // GRAPHITE_DEBUG

	// Create the instance.
	GRAPHITE_VK_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance), "Failed to create the instance!");

	// Load the instance functions.
	volkLoadInstance(m_Instance);

#ifdef GRAPHITE_DEBUG
	// Open the log file.
	m_LogFile = std::ofstream("VulkanLogs.txt");

	// Create the debugger.
	const auto vkCreateDebugUtilsMessengerEXT = GRAPHITE_BIT_CAST(PFN_vkCreateDebugUtilsMessengerEXT, vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
	GRAPHITE_VK_ASSERT(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_DebugMessenger), "Failed to create the debug messenger.");

#endif // GRAPHITE_DEBUG
}

void Instance::selectPhysicalDevice()
{
	// Enumerate physical devices.
	uint32_t deviceCount = 0;
	GRAPHITE_VK_ASSERT(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr), "Failed to enumerate physical devices.");

	// Throw an error if there are no physical devices available.
	if (deviceCount == 0)
	{
		GRAPHITE_LOG_FATAL("No physical devices found!");
		return;
	}

	std::vector<VkPhysicalDevice> candidates(deviceCount);
	GRAPHITE_VK_ASSERT(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, candidates.data()), "Failed to enumerate physical devices.");

	struct Candidate { VkPhysicalDeviceProperties m_Properties; VkPhysicalDevice m_Candidate; };
	std::array<Candidate, 6> priorityMap = { Candidate() };

	// Iterate through all the candidate devices and find the best device.
	for (const auto& candidate : candidates)
	{
		// Check if the device is suitable for our use.
		if (CheckDeviceExtensionSupport(candidate, m_DeviceExtensions) &&
			CheckQueueSupport(candidate, VK_QUEUE_GRAPHICS_BIT) &&
			CheckQueueSupport(candidate, VK_QUEUE_COMPUTE_BIT) &&
			CheckQueueSupport(candidate, VK_QUEUE_TRANSFER_BIT))
		{
			VkPhysicalDeviceProperties physicalDeviceProperties = {};
			vkGetPhysicalDeviceProperties(candidate, &physicalDeviceProperties);

			// Sort the candidates by priority.
			uint8_t priorityIndex = 5;
			switch (physicalDeviceProperties.deviceType)
			{
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				priorityIndex = 0;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				priorityIndex = 1;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				priorityIndex = 2;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				priorityIndex = 3;
				break;

			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				priorityIndex = 4;
				break;

			default:
				priorityIndex = 5;
				break;
			}

			priorityMap[priorityIndex].m_Candidate = candidate;
			priorityMap[priorityIndex].m_Properties = physicalDeviceProperties;
		}
	}

	// Choose the physical device with the highest priority.
	for (const auto& candidate : priorityMap)
	{
		if (candidate.m_Candidate != VK_NULL_HANDLE)
		{
			m_PhysicalDevice = candidate.m_Candidate;
			m_PhysicalDeviceProperties = candidate.m_Properties;
			break;
		}
	}

	// Check if we found a physical device.
	if (m_PhysicalDevice.getUnsafe() == VK_NULL_HANDLE)
	{
		GRAPHITE_LOG_FATAL("Could not find a physical device with the required requirements!");
		return;
	}

	// Get the device details.
	GRAPHITE_LOG_INFORMATION("Device API Version: {}", m_PhysicalDeviceProperties.apiVersion);
	GRAPHITE_LOG_INFORMATION("Device Driver Version: {}", m_PhysicalDeviceProperties.driverVersion);
	GRAPHITE_LOG_INFORMATION("Device Name: {}", m_PhysicalDeviceProperties.deviceName);

	// Get the unsupported render target types.
	const auto unsupportedExtensions = GetUnsupportedDeviceExtensions(m_PhysicalDevice.getUnsafe(), m_DeviceExtensions);
	for (const auto& extension : unsupportedExtensions)
	{
		GRAPHITE_LOG_INFORMATION("The {} extension is not supported and therefore will not be used.", extension.data());

#ifdef GRAPHITE_FEATURE_RANGES
		auto ret = std::ranges::remove(m_DeviceExtensions, extension);
		m_DeviceExtensions.erase(ret.begin());

#else
		auto ret = std::remove(m_DeviceExtensions.begin(), m_DeviceExtensions.end(), extension);
		m_DeviceExtensions.erase(ret);

#endif
	}

	// Setup the queue families.
	getGraphicsQueue().getUnsafe().m_Family = FindPhysialDeviceQueueFamily(m_PhysicalDevice.getUnsafe(), VK_QUEUE_GRAPHICS_BIT);
	getComputeQueue().getUnsafe().m_Family = FindPhysialDeviceQueueFamily(m_PhysicalDevice.getUnsafe(), VK_QUEUE_COMPUTE_BIT);
	getTransferQueue().getUnsafe().m_Family = FindPhysialDeviceQueueFamily(m_PhysicalDevice.getUnsafe(), VK_QUEUE_TRANSFER_BIT);
}

void Instance::createLogicalDevice()
{
	// Setup device queues.
	constexpr float priority = 1.0f;

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = nullptr;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &priority;
	queueCreateInfo.queueFamilyIndex = getGraphicsQueue().getUnsafe().m_Family;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = { queueCreateInfo };
	if (getComputeQueue().getUnsafe().m_Family != getGraphicsQueue().getUnsafe().m_Family)
	{
		queueCreateInfo.queueFamilyIndex = getComputeQueue().getUnsafe().m_Family;
		queueCreateInfos.emplace_back(queueCreateInfo);
	}

	if (getTransferQueue().getUnsafe().m_Family != getComputeQueue().getUnsafe().m_Family)
	{
		queueCreateInfo.queueFamilyIndex = getTransferQueue().getUnsafe().m_Family;
		queueCreateInfos.emplace_back(queueCreateInfo);
	}

	// Setup all the required features.
	VkPhysicalDeviceFeatures features = {};
	features.samplerAnisotropy = VK_TRUE;
	features.sampleRateShading = VK_TRUE;
	features.tessellationShader = VK_TRUE;
	features.geometryShader = VK_TRUE;
	features.fragmentStoresAndAtomics = VK_TRUE;
	features.fillModeNonSolid = VK_TRUE;

	// Setup the device create info.
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &features;

#ifdef GRAPHITE_DEBUG
	// Get the validation layers and initialize it.
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
	deviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();

#endif // GRAPHITE_DEBUG

	// Create the device.
	GRAPHITE_VK_ASSERT(vkCreateDevice(m_PhysicalDevice.getUnsafe(), &deviceCreateInfo, nullptr, &m_LogicalDevice.getUnsafe()), "Failed to create the logical device!");

	// Load the device table.
	volkLoadDeviceTable(&m_DeviceTable, m_LogicalDevice.getUnsafe());

	// Get the queues.
	for (auto& queue : m_Queues)
		m_DeviceTable.vkGetDeviceQueue(m_LogicalDevice.getUnsafe(), queue.getUnsafe().m_Family, 0, &queue.getUnsafe().m_Queue);
}

void Instance::createMemoryAllocator()
{
	// Setup the Vulkan functions needed by VMA.
	VmaVulkanFunctions functions = {};
	functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
	functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	functions.vkAllocateMemory = m_DeviceTable.vkAllocateMemory;
	functions.vkFreeMemory = m_DeviceTable.vkFreeMemory;
	functions.vkMapMemory = m_DeviceTable.vkMapMemory;
	functions.vkUnmapMemory = m_DeviceTable.vkUnmapMemory;
	functions.vkFlushMappedMemoryRanges = m_DeviceTable.vkFlushMappedMemoryRanges;
	functions.vkInvalidateMappedMemoryRanges = m_DeviceTable.vkInvalidateMappedMemoryRanges;
	functions.vkBindBufferMemory = m_DeviceTable.vkBindBufferMemory;
	functions.vkBindImageMemory = m_DeviceTable.vkBindImageMemory;
	functions.vkGetBufferMemoryRequirements = m_DeviceTable.vkGetBufferMemoryRequirements;
	functions.vkGetImageMemoryRequirements = m_DeviceTable.vkGetImageMemoryRequirements;
	functions.vkCreateBuffer = m_DeviceTable.vkCreateBuffer;
	functions.vkDestroyBuffer = m_DeviceTable.vkDestroyBuffer;
	functions.vkCreateImage = m_DeviceTable.vkCreateImage;
	functions.vkDestroyImage = m_DeviceTable.vkDestroyImage;
	functions.vkCmdCopyBuffer = m_DeviceTable.vkCmdCopyBuffer;
	functions.vkGetBufferMemoryRequirements2KHR = m_DeviceTable.vkGetBufferMemoryRequirements2KHR;
	functions.vkGetImageMemoryRequirements2KHR = m_DeviceTable.vkGetImageMemoryRequirements2KHR;
	functions.vkBindBufferMemory2KHR = m_DeviceTable.vkBindBufferMemory2KHR;
	functions.vkBindImageMemory2KHR = m_DeviceTable.vkBindImageMemory2KHR;
	functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
	functions.vkGetDeviceBufferMemoryRequirements = m_DeviceTable.vkGetDeviceBufferMemoryRequirements;
	functions.vkGetDeviceImageMemoryRequirements = m_DeviceTable.vkGetDeviceImageMemoryRequirements;

	// Setup create info.
	VmaAllocatorCreateInfo createInfo = {};
	createInfo.flags = VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT /*| VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT*/;
	createInfo.physicalDevice = m_PhysicalDevice.getUnsafe();
	createInfo.device = m_LogicalDevice.getUnsafe();
	createInfo.pVulkanFunctions = &functions;
	createInfo.instance = m_Instance;
	createInfo.vulkanApiVersion = volkGetInstanceVersion();

	// Create the allocator.
	GRAPHITE_VK_ASSERT(vmaCreateAllocator(&createInfo, &m_Allocator.getUnsafe()), "Failed to create the allocator!");
}
