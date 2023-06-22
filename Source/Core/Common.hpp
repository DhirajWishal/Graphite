// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "Features.hpp"

#ifdef GRAPHITE_FEATURE_RANGES
#define GRAPHITE_RANGES(function, container, ...)	std::ranges::function(container, __VA_ARGS__)

#else
#define GRAPHITE_RANGES(function, container, ...)	std::function(container.begin(), container.end(), __VA_ARGS__)

#endif

#ifdef GRAPHITE_FEATURE_BIT_CAST
#define GRAPHITE_BIT_CAST(from, to)					std::bit_cast<to>(from)

#else
#define GRAPHITE_BIT_CAST(from, to)					reinterpret_cast<to>(from)

#endif