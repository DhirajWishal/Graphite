// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "InstanceBoundObject.hpp"

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
	 * @param width The width of the image.
	 * @param height The height of the image.
	 * @param depth The depth of the image.
	 * @param format The image format to use.
	 */
	explicit Image(Instance& instance, uint32_t width, uint32_t height, uint32_t depth, VkFormat format);

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