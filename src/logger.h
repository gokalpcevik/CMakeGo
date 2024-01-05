#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace CMakeGo {
extern std::shared_ptr<spdlog::logger> g_core_logger;
void InitLogger();
inline std::shared_ptr<spdlog::logger> GetLogger() { return g_core_logger; }
} // namespace CMakeGo

#define CGO_Trace(...) SPDLOG_LOGGER_TRACE(::CMakeGo::GetLogger(), __VA_ARGS__)
#define CGO_Info(...) SPDLOG_LOGGER_INFO(::CMakeGo::GetLogger(), __VA_ARGS__)
#define CGO_Error(...) SPDLOG_LOGGER_ERROR(::CMakeGo::GetLogger(), __VA_ARGS__)
#define CGO_Warn(...) SPDLOG_LOGGER_WARN(::CMakeGo::GetLogger(), __VA_ARGS__)
#define CGO_Debug(...) SPDLOG_LOGGER_DEBUG(::CMakeGo::GetLogger(), __VA_ARGS__)
#define CGO_Critical(...) SPDLOG_LOGGER_CRITICAL(::CMakeGo::GetLogger(), __VA_ARGS__)
