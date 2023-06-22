// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "InstanceBoundObject.hpp"

/**
 * Image class.
 * This is the base class for all the supported images of the engine.
 */
class Image : public InstanceBoundObject
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

	/**
	 * Get the width of the image.
	 *
	 * @return The width of the image.
	 */
	[[nodiscard]] uint32_t getWidth() const { return m_Width; }

	/**
	 * Get the height of the image.
	 *
	 * @return The height of the image.
	 */
	[[nodiscard]] uint32_t getHeight() const { return m_Height; }

	/**
	 * Get the depth of the image.
	 *
	 * @return The depth of the image.
	 */
	[[nodiscard]] uint32_t getDepth() const { return m_Depth; }

	/**
	 * Get the format of the image.
	 *
	 * @return The format of the image.
	 */
	[[nodiscard]] VkFormat getFormat() const { return m_Format; }

private:
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
	uint32_t m_Depth = 0;

	VkFormat m_Format = VK_FORMAT_UNDEFINED;

	VkImage m_Image = VK_NULL_HANDLE;
	VmaAllocation m_ImageMemory = nullptr;
};