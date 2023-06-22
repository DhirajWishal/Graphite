// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include <vulkan/vulkan.hpp>

/**
 * Instance class.
 * This contains the main Vulkan instance which is used by the engine.
 */
class Instance final
{
public:
	/**
	 * Default constructor.
	 */
	Instance();

private:
	vk::Instance m_Insatnce;
	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_Device;
};