// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "InstanceBoundObject.hpp"

/**
 * Buffer class.
 * This class contains a single Vulkan buffer object.
 */
class Buffer final : public InstanceBoundObject
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 * @param size The size of the buffer.
	 * @param usage The buffer usage.
	 */
	explicit Buffer(Instance& instance, uint64_t size, VkBufferUsageFlags usage);

	/**
	 * Destructor.
	 */
	~Buffer() override;

public:
	GRAPHITE_SETUP_SIMPLE_GETTER(uint64_t, Size, m_Size);
	GRAPHITE_SETUP_SIMPLE_GETTER(VkBuffer, Buffer, m_Buffer);
	GRAPHITE_SETUP_SIMPLE_GETTER(VmaAllocation, BufferMemory, m_BufferMemory);

private:
	uint64_t m_Size = 0;

	VkBuffer m_Buffer = VK_NULL_HANDLE;
	VmaAllocation m_BufferMemory = nullptr;
};