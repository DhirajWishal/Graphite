// Copyright (c) 2023 Dhiraj Wishal

#include "Buffer.hpp"

Buffer::Buffer(Instance& instance, uint64_t size)
	: InstanceBoundObject(instance), m_Size(size)
{
}
