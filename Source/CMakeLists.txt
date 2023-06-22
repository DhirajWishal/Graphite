# Copyright (c) 2023 Dhiraj Wishal

# Set the basic project information.
project(
	Graphite
	VERSION 1.0.0
	DESCRIPTION "Graphite engine."
)

# Set the sources.
file(GLOB_RECURSE SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
	*.hlsl
	*.hlsli

	*.hpp
)

set(
	SOURCES

	"Main.cpp"
	"Application.cpp"
	"Application.hpp"

	"Core/Logging.hpp"
	"Core/Features.hpp"
	"Core/Guarded.hpp"

	"Backend/Instance.cpp"
	"Backend/Instance.hpp"

	${SHADERS}
)

# Add the source group.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Add the library.
add_executable(
	Graphite

	${SOURCES}
)

# Set the include directories.
target_include_directories(
	Graphite 

	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${IMGUI_INCLUDE_DIR}
	PRIVATE ${TINYGLTF_INCLUDE_DIR}
	PRIVATE ${SPDLOG_INCLUDE_DIR}
	PRIVATE ${XXHASH_INCLUDE_DIR}
	PRIVATE ${OPTICK_INCLUDE_DIR}
	PRIVATE ${VULKAN_HEADERS_INCLUDE_DIR}
	PRIVATE ${VOLK_INCLUDE_DIR}
	PRIVATE ${VMA_INCLUDE_DIR}
)

# Add the target links.
target_link_libraries(Graphite GraphiteThirdParty_ImGui GraphiteThirdParty_Optick GraphiteThirdParty_volk SDL3-shared)

# Make sure to specify the C++ standard to C++20.
set_property(TARGET Graphite PROPERTY CXX_STANDARD 20)

# Copy the SDL shared library to the output directory.
add_custom_command(
	TARGET Graphite 
	POST_BUILD

	COMMENT "Copying SDL shared library to the output directory."
	COMMAND ${CMAKE_COMMAND} -E copy
		$<TARGET_FILE:SDL3-shared>
		$<TARGET_FILE_DIR:Graphite>/$<TARGET_FILE_NAME:SDL3-shared>
)

# If we are on MSVC, we can use the Multi Processor Compilation option.
if (MSVC)
	target_compile_options(Graphite PRIVATE "/MP")	
endif ()