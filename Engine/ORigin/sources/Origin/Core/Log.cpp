// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/Log.h"

#include "ConsoleManager.h"

namespace origin {
static Log *s_Instance = nullptr;
Log::Log()
{
    s_Instance = this;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
}

void Log::PrintColoredMessage(const std::string &message, const LogLevel level)
{
    const char *color_code;
    const char *level_str;

    const std::string &time_string = ConsoleManager::GetCurrentTime();

    switch (level)
    {
    case LogLevel::Critical:
    case LogLevel::Error:
        color_code = "\033[1;31m";
        level_str = "[Error]\t";
        break;
    case LogLevel::Warning:
        color_code = "\033[1;33m";
        level_str = "[Warn]\t";
        break;
    default:
        color_code = "\033[1;37m";
        level_str = "[Info]\t";
        break;
    }
    if (m_Buffer.tellp() > 1024)
    {
        m_Buffer.str("");
        m_Buffer.clear();
    }

    m_Buffer << color_code << time_string << /* level_str << */ " " << message << "\033[0m\n";
    //m_Buffer << message << "\n";
    std::cout << m_Buffer.rdbuf();
    m_Buffer.str("");
    m_Buffer.clear();
}

Log *Log::GetInstance()
{
    return s_Instance;
}

}
