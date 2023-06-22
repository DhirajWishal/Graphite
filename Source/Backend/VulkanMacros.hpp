// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include "Core/Logging.hpp"

#ifdef GRAPHITE_DEBUG
#define GRAPHITE_VK_ASSERT(function, ...)	if (function != VK_SUCCESS) GRAPHITE_LOG_FATAL(__VA_ARGS__)

#else
#define GRAPHITE_VK_ASSERT(function, ...)	function

#endif