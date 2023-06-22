// Copyright (c) 2023 Dhiraj Wishal

#include "Image.hpp"

Image::Image(Instance& instance, uint32_t width, uint32_t height, uint32_t depth, VkFormat format)
	: InstanceBoundObject(instance), m_Width(width), m_Height(height), m_Depth(depth), m_Format(format)
{
}
