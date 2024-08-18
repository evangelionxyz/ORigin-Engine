// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <vector>
#include <chrono>
#include <sstream>

#if defined(OGN_PLATFORM_WINDOWS)
    #include <format>
#elif defined(OGN_PLATFORM_LINUX)
    #include <fmt/format.h>
#endif

#include "Base.h"

namespace origin
{
    enum ConsoleMessageType
    {
        Info    = BIT(1),
        Warning = BIT(2),
        Error   = BIT(3)
    };

    struct ConsoleMessage
    {
        std::string Message;
        std::string Timestamp;
        ConsoleMessageType Type = ConsoleMessageType::Info;
    };

    class ConsoleManager
    {
    public:
        ConsoleManager();
        ~ConsoleManager();

        std::string GetCurrentTime();
        void PushMessage(ConsoleMessageType type, const std::string &message);
        void Clear();

        static ConsoleManager &Get();

        template<typename... Args>
        void PushFormattedMessage(ConsoleMessageType type, fmt::format_string<Args...> format, Args&&... args)
        {
            std::string message = fmt::format(format, std::forward<Args>(args)...);
            PushMessage(type, message);
        }

        void PushFormattedMessage(ConsoleMessageType type, const std::string &message)
        {
            PushMessage(type, message);
        }

        static std::vector<ConsoleMessage> &GetMessages();
    private:
        std::vector<ConsoleMessage> m_Messages;
    };
}

#define PUSH_CONSOLE_WARNING(...) origin::ConsoleManager::Get().PushFormattedMessage(origin::ConsoleMessageType::Warning, __VA_ARGS__)
#define PUSH_CONSOLE_INFO(...)    origin::ConsoleManager::Get().PushFormattedMessage(origin::ConsoleMessageType::Info, __VA_ARGS__)
#define PUSH_CONSOLE_ERROR(...)   origin::ConsoleManager::Get().PushFormattedMessage(origin::ConsoleMessageType::Error, __VA_ARGS__)
