// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "InstanceBoundObject.hpp"

#include <vector>

/**
 * Image builder class.
 */
struct ImageBuilder final
{
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, uint32_t, Width) = 0;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, uint32_t, Height) = 0;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, uint32_t, Depth) = 1;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, VkImageType, Type) = VK_IMAGE_TYPE_2D;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, VkImageUsageFlags, Usage) = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, VkSampleCountFlagBits, Samples) = VK_SAMPLE_COUNT_1_BIT;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, uint32_t, Layers) = 1;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, bool, EnableMipMaps) = true;
	GRAPHITE_SETUP_CHAIN_ENTRY(ImageBuilder, bool, IsCubeMap) = false;
};

/**
 * Image class.
 * This is the base class for all the supported images of the engine.
 */
class Image final : public InstanceBoundObject
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 * @param builder The image builder structure.
	 * @param format The image format to use.
	 */
	explicit Image(Instance& instance, const ImageBuilder& builder, VkFormat format);

	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 * @param builder The image builder structure.
	 * @param formats The candidate image formats to use. The class will find the best format from the given list. Make sure to have the formats in the best to worst order.
	 */
	explicit Image(Instance& instance, const ImageBuilder& builder, std::vector<VkFormat> formats);

public:
	GRAPHITE_SETUP_SIMPLE_GETTER(uint32_t, Width, m_Width);
	GRAPHITE_SETUP_SIMPLE_GETTER(uint32_t, Height, m_Height);
	GRAPHITE_SETUP_SIMPLE_GETTER(uint32_t, Depth, m_Depth);
	GRAPHITE_SETUP_SIMPLE_GETTER(VkFormat, Format, m_Format);
	GRAPHITE_SETUP_SIMPLE_GETTER(VkImage, Image, m_Image);
	GRAPHITE_SETUP_SIMPLE_GETTER(VmaAllocation, ImageMemory, m_ImageMemory);

private:
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_Depth = 0;

	VkFormat m_Format = VK_FORMAT_UNDEFINED;

	VkImage m_Image = VK_NULL_HANDLE;
	VmaAllocation m_ImageMemory = nullptr;
};