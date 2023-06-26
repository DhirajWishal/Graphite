// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "Features.hpp"

#ifdef GRAPHITE_FEATURE_RANGES
#define GRAPHITE_RANGES(function, container, ...)	std::ranges::function(container, __VA_ARGS__)

#else
#define GRAPHITE_RANGES(function, container, ...)	std::function(container.begin(), container.end(), __VA_ARGS__)

#endif

#ifdef GRAPHITE_FEATURE_BIT_CAST
#define GRAPHITE_BIT_CAST(to, from)					std::bit_cast<to>(from)

#else
#define GRAPHITE_BIT_CAST(to, from)					reinterpret_cast<to>(from)

#endif

#define GRAPHITE_SETUP_SIMPLE_GETTER(type, name, variable)	[[nodiscard]] type get##name() const { return variable; }

#define GRAPHITE_SETUP_GETTERS(type, name, variable)					\
	[[nodiscard]] const type& get##name() const { return variable; }	\
	[[nodiscard]] type& get##name() { return variable; }

#define GRAPHITE_DISABLE_COPY_AND_MOVE(object)							\
	object(const object&) = delete;										\
	object(object&&) noexcept = delete;									\
	object& operator=(const object&) = delete;							\
	object& operator=(object&&) noexcept = delete

#define GRAPHITE_SETUP_CHAIN_ENTRY(object, type, name)							\
	const type& get##name() const { return m_##name; }							\
	object& set##name(const type& value) { m_##name = value; return *this; }	\
	type m_##name