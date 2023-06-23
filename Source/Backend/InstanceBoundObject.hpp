// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "Core/Common.hpp"

#include <cstdint>

#ifndef VK_NO_PROTOTYPES
#	define VK_NO_PROTOTYPES

#endif

#include <vk_mem_alloc.h>

class Instance;

/**
 * Instance bound object class.
 * This class is the base class for all the objects that are based on the Instance class.
 */
class InstanceBoundObject
{
public:
	/**
	 * Explicit constructor.
	 *
	 * @param instance The instance reference.
	 */
	explicit InstanceBoundObject(Instance& instance) : m_Instance(instance) {}

	/**
	 * Virtual default destructor.
	 */
	virtual ~InstanceBoundObject() = default;

	/**
	 * Get the stored instance object reference.
	 *
	 * @return The object reference.
	 */
	[[nodiscard]] Instance& getInstance() { return m_Instance; }

	/**
	 * Get the stored instance object reference.
	 *
	 * @return The object reference.
	 */
	[[nodiscard]] const Instance& getInstance() const { return m_Instance; }

	GRAPHITE_DISABLE_COPY_AND_MOVE(InstanceBoundObject);

protected:
	Instance& m_Instance;
};