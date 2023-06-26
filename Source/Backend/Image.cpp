// Copyright (c) 2023 Dhiraj Wishal

#include "Image.hpp"
#include "Instance.hpp"
#include "VulkanMacros.hpp"

Image::Image(Instance& instance, const ImageBuilder& builder, VkFormat format)
	: InstanceBoundObject(instance), m_Width(builder.m_Width), m_Height(builder.m_Height), m_Depth(builder.m_Depth), m_Format(format)
{
	// Create the image.
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = builder.m_IsCubeMap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType = builder.m_Type;
	imageCreateInfo.extent.width = m_Width;
	imageCreateInfo.extent.height = m_Height;
	imageCreateInfo.extent.depth = m_Depth;
	imageCreateInfo.mipLevels = builder.m_EnableMipMaps ? static_cast<uint32_t>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1 : 1;
	imageCreateInfo.arrayLayers = builder.m_Layers;
	imageCreateInfo.samples = builder.m_Samples;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = builder.m_Usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.format = format;

	VmaAllocationCreateInfo allocationCreateInfo = {};
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

	instance.getAllocator().access([this, imageCreateInfo, allocationCreateInfo](VmaAllocator allocator)
		{
			GRAPHITE_VK_ASSERT(vmaCreateImage(allocator, &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_ImageMemory, nullptr), "Failed to create the image!");
		}
	);
}

Image::Image(Instance& instance, const ImageBuilder& builder, std::vector<VkFormat> formats)
	: InstanceBoundObject(instance), m_Width(builder.m_Width), m_Height(builder.m_Height), m_Depth(builder.m_Depth)
{
	// Create the image.
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = builder.m_IsCubeMap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType = builder.m_Type;
	imageCreateInfo.extent.width = m_Width;
	imageCreateInfo.extent.height = m_Height;
	imageCreateInfo.extent.depth = m_Depth;
	imageCreateInfo.mipLevels = builder.m_EnableMipMaps ? static_cast<uint32_t>(std::floor(std::log2(std::max(m_Width, m_Height)))) + 1 : 1;
	imageCreateInfo.arrayLayers = builder.m_Layers;
	imageCreateInfo.samples = builder.m_Samples;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = builder.m_Usage;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// Resolve the image format.
	bool formatFound = false;
	for (const auto candidate : formats)
	{
		imageCreateInfo.format = candidate;

		// Get the format properties.
		VkImageFormatProperties formatProperties = {};
		VkResult result = VK_ERROR_UNKNOWN;
		instance.getPhysicalDevice().access([&formatProperties, &imageCreateInfo, &result](VkPhysicalDevice physicalDevice)
			{
				result = vkGetPhysicalDeviceImageFormatProperties(
					physicalDevice,
					imageCreateInfo.format,
					imageCreateInfo.imageType,
					imageCreateInfo.tiling,
					imageCreateInfo.usage,
					imageCreateInfo.flags,
					&formatProperties
				);
			});

		// If the format is supported, we can go with it.
		if (result == VK_SUCCESS)
		{
			m_Format = candidate;
			formatFound = true;
			break;
		}
	}

	// Check if we found a format.
	if (!formatFound)
	{
		GRAPHITE_LOG_FATAL("The provided format (with or without candidates) cannot be used to create the image!");
		return;
	}

	VmaAllocationCreateInfo allocationCreateInfo = {};
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

	instance.getAllocator().access([this, imageCreateInfo, allocationCreateInfo](VmaAllocator allocator)
		{
			GRAPHITE_VK_ASSERT(vmaCreateImage(allocator, &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_ImageMemory, nullptr), "Failed to create the image!");
		}
	);

}
