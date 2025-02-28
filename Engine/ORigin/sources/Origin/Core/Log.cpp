// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "Log.h"

namespace origin {

void Log::Init()
{
    spdlog::init_thread_pool(8192, 1);

    s_console_sink = CreateRef<spdlog::sinks::stdout_color_sink_mt>();
    s_console_sink->set_pattern("%^[%T] %n: %v%$");

    // Engine
    s_core_logger = CreateRef<spdlog::async_logger>(
        "[origin]",
        s_console_sink,
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block
    );

    s_core_logger->set_level(spdlog::level::trace);

    s_client_logger = CreateRef<spdlog::async_logger>(
        "[client]",
        s_console_sink,
        spdlog::thread_pool(),
        spdlog::async_overflow_policy::block
    );
    s_client_logger->set_level(spdlog::level::trace);
}

void Log::Shutdown()
{
    spdlog::shutdown();
}

Ref<spdlog::async_logger> Log::s_core_logger;
Ref<spdlog::async_logger> Log::s_client_logger;
Ref<spdlog::sinks::stdout_color_sink_mt> Log::s_console_sink;

}