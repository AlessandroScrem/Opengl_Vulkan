#pragma once
#define  SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

enum class EngineType
{
    Opengl,
    Vulkan
};