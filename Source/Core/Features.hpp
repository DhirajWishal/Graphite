// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include <version>

// Macro to check if a feature is available by comparing it to an expected value.
#define GRAPHITE_CHECK_FEATURE(feature, value)	feature == value

// Check and define the GRAPHITE_FEATURE_SOURCE_LOCATION macro if the std::source_location class is available.
#ifdef __cpp_lib_source_location
#	if GRAPHITE_CHECK_FEATURE(__cpp_lib_source_location, 201907L)
// The compiler supports the' __cpp_lib_source_location' feature (has support for std::source_location).
#		define GRAPHITE_FEATURE_SOURCE_LOCATION
#	endif
#endif