// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef CONSOLE_MANAGER_H
#define CONSOLE_MANAGER_H

#include "ORigin/Utils/StringUtils.h"
#include <vector>

namespace origin {
struct ConsoleMessage
{
    std::string Message;
    std::string Timestamp;
    LogLevel Level = LogLevel::Info;
};

class ConsoleManager
{
public:
    ConsoleManager();
    ~ConsoleManager();

    static std::string GetCurrentTime();
    void PushMessage(LogLevel level, const std::string &message);
    void Clear();

    static ConsoleManager &Get();

    template<typename... Args>
    void PushFormattedMessage(const LogLevel level, const char *format, Args&&... args)
    {
        const std::string message = Utils::FormatString(format, std::forward<Args>(args)...);
        PushMessage(level, message);
    }

    void PushFormattedMessage(LogLevel level, const std::string &message);

    static std::vector<ConsoleMessage> &GetMessages();
private:
    std::vector<ConsoleMessage> m_Messages;
};
}

#define PUSH_CONSOLE_WARNING(...) origin::ConsoleManager::Get().PushFormattedMessage(origin::LogLevel::Warning, __VA_ARGS__)
#define PUSH_CONSOLE_INFO(...)    origin::ConsoleManager::Get().PushFormattedMessage(origin::LogLevel::Info, __VA_ARGS__)
#define PUSH_CONSOLE_ERROR(...)   origin::ConsoleManager::Get().PushFormattedMessage(origin::LogLevel::Error, __VA_ARGS__)

#endif