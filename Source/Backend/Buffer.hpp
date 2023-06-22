// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "InstanceBoundObject.hpp"

/**
 * Buffer class.
 * This class contains a single Vulkan buffer object.
 */
class Buffer : public InstanceBoundObject
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 * @param size The size of the buffer.s
	 */
	explicit Buffer(Instance& instance, uint64_t size);

	/**
	 * Get the size of the buffer.
	 *
	 * @return The size in bytes.
	 */
	[[nodiscard]] uint64_t getSize() const { return m_Size; }

private:
	uint64_t m_Size = 0;

	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VmaAllocation m_BufferMemory = nullptr;
};