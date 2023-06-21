// Copyright (c) 2023 Dhiraj Wishal

#pragma once

#include <spdlog/spdlog.h>


#ifdef __cpp_lib_source_location
#	if __cpp_lib_source_location == 201907L
#include <source_location>

/**
 * Log a trace to the console.
 *
 * @tparam Message The message type.
 * @param location The source location.
 * @param message The message to log.
 */
template<class Message>
void TraceLog(std::source_location&& location, Message&& message)
{
	spdlog::info("[Trace \"{}\":{}] {}", location.file_name(), location.line(), std::move(message));
}

#		define GRAPHITE_TRACE_FUNCTION(format, ...)	::TraceLog(std::source_location::current(), fmt::format(format, __VA_ARGS__))

#endif

#else
#	define GRAPHITE_TRACE_FUNCTION(format, ...)	::spdlog::info("[Trace \"{}\":{}] {}", __FILE__, __LINE__, fmt::format(format, __VA_ARGS__))

#endif

/**
 * Graphite no-op function.
 * This function compiles down to nothing and the compiler can optimize it out when needed.
 *
 * @tparam Arguments The argument types that can be passed to the function.
 */
template<class... Arguments>
constexpr void NoOp(Arguments&&...) noexcept {}

/**
 * Graphite log level defines the types of logging that can be done by the engine.
 * The levels are:
 * 1 - Fatal logs only.
 * 2 - Error logs and the log types before this (1).
 * 3 - Warning logs and the log types before this (1, 2).
 * 4 - Information logs and the log types before this (1, 2, 3).
 * 5 - Trace logs and the log types before this (1, 2, 3, 4).
 *
 * If a log level is not defined, no logs will be printed.
 */
#ifdef GRAPHITE_LOG_LEVEL
#	if GRAPHITE_LOG_LEVEL > 0
#		define GRAPHITE_LOG_FATAL(...)								::spdlog::critical(__VA_ARGS__)

#		if GRAPHITE_LOG_LEVEL > 1
#			define GRAPHITE_LOG_ERROR(...)							::spdlog::error(__VA_ARGS__)

#			if GRAPHITE_LOG_LEVEL > 2
#				define GRAPHITE_LOG_WARNING(...)					::spdlog::warn(__VA_ARGS__)

#				if GRAPHITE_LOG_LEVEL > 3
#					define GRAPHITE_LOG_INFORMATION(...)			::spdlog::info(__VA_ARGS__)

#					if GRAPHITE_LOG_LEVEL > 4
#						define GRAPHITE_LOG_TRACE(msg,...)			GRAPHITE_TRACE_FUNCTION(msg, __VA_ARGS__)

#					endif
#				endif
#			endif
#		endif
#	endif
#endif // GRAPHITE_LOG_LEVEL

#ifndef GRAPHITE_LOG_FATAL
#	define GRAPHITE_LOG_FATAL(...)									::NoOp()
#endif

#ifndef GRAPHITE_LOG_ERROR
#	define GRAPHITE_LOG_ERROR(...)									::NoOp()
#endif

#ifndef GRAPHITE_LOG_WARNING
#	define GRAPHITE_LOG_WARNING(...)								::NoOp()
#endif

#ifndef GRAPHITE_LOG_INFORMATION
#	define GRAPHITE_LOG_INFORMATION(...)							::NoOp()
#endif

#ifndef GRAPHITE_LOG_TRACE
#	define GRAPHITE_LOG_TRACE(...)									::NoOp()
#endif

#ifdef GRAPHITE_DEBUG
#	define GRAPHITE_LOG_DEBUG(...)									SPDLOG_DEBUG(__VA_ARGS__)

#	ifdef GRAPHITE_PLATFORM_WINDOWS
#		define GRAPHITE_DEBUG_BREAK									__debugbreak()

#	endif

#else
#	define GRAPHITE_LOG_DEBUG(...)									::NoOp()
#	define GRAPHITE_DEBUG_BREAK										::NoOp()

#endif // GRAPHITE_DEBUG

#define GRAPHITE_ASSERT(condition, ...)								if (!(condition)) (GRAPHITE_LOG_FATAL(__VA_ARGS__), GRAPHITE_DEBUG_BREAK)

#define GRAPHITE_TODO(_day, _month, _year, ...)																											\
	if (std::chrono::year(_year)/_month/_day >= std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())))	\
		GRAPHITE_LOG_TRACE("TODO: " __VA_ARGS__)

#define GRAPHITE_FIXME(_day, _month, _year, ...)																										\
	if (std::chrono::year(_year)/_month/_day >= std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())))	\
		GRAPHITE_LOG_TRACE("FIXME: " __VA_ARGS__)

#define GRAPHITE_TODO_NOW(...)										GRAPHITE_LOG_TRACE("TODO: " __VA_ARGS__)
#define GRAPHITE_FIXME_NOW(...)										GRAPHITE_LOG_TRACE("FIXME: " __VA_ARGS__)