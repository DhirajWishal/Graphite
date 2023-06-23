// Copyright (c) 2023 Dhiraj Wishal

#include "Buffer.hpp"
#include "Instance.hpp"
#include "VulkanMacros.hpp"

Buffer::Buffer(Instance& instance, uint64_t size, VkBufferUsageFlags usage)
	: InstanceBoundObject(instance), m_Size(size)
{
	VmaAllocationCreateFlags vmaFlags = 0;
	VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO;

	if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT || usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
	{
		memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	}
	else
	{
		vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
	}

	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.size = size;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;

	VmaAllocationCreateInfo allocationCreateInfo = {};
	allocationCreateInfo.flags = vmaFlags;
	allocationCreateInfo.usage = memoryUsage;

	m_Instance.getAllocator().access([this, &createInfo, &allocationCreateInfo](VmaAllocator allocator)
		{
			GRAPHITE_VK_ASSERT(vmaCreateBuffer(allocator, &createInfo, &allocationCreateInfo, &m_Buffer, &m_BufferMemory, nullptr), "Failed to create the buffer!");
		}
	);
}

Buffer::~Buffer()
{
	m_Instance.getAllocator().access([this](VmaAllocator allocator)
		{
			vmaDestroyBuffer(allocator, m_Buffer, m_BufferMemory);
		}
	);
}
