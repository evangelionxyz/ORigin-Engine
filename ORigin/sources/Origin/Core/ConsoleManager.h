// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <vector>
#include <chrono>
#include <format>

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
        void PushFormattedMessage(ConsoleMessageType type, const std::string &format, Args&&... args)
        {
            std::string message = std::vformat(format, std::make_format_args(args...));
            PushMessage(type, message);
        }

        static std::vector<ConsoleMessage> &GetMessages();
    private:
        std::vector<ConsoleMessage> m_Messages;
    };
}

#define PUSH_CONSOLE_WARNING(format, ...) origin::ConsoleManager::Get().PushFormattedMessage(origin::ConsoleMessageType::Warning, format, __VA_ARGS__)
#define PUSH_CONSOLE_INFO(format, ...)    origin::ConsoleManager::Get().PushFormattedMessage(origin::ConsoleMessageType::Info, format, __VA_ARGS__)
#define PUSH_CONSOLE_ERROR(format, ...)   origin::ConsoleManager::Get().PushFormattedMessage(origin::ConsoleMessageType::Error, format, __VA_ARGS__)
