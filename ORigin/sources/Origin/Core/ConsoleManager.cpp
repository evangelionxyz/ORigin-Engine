// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ConsoleManager.h"

namespace origin
{
    static ConsoleManager *s_Instance = nullptr;

    ConsoleManager::ConsoleManager()
    {
        s_Instance = this;
    }

    ConsoleManager::~ConsoleManager()
    {
        Clear();
    }

    std::string ConsoleManager::GetCurrentTime()
    {
        const auto now = std::chrono::system_clock::now();
        const auto time_t_now = std::chrono::system_clock::to_time_t(now);
        const std::tm local_tm = *std::localtime(&time_t_now);

        std::ostringstream oss;
        oss << std::put_time(&local_tm, "[%H:%M:%S]");
        return oss.str();
    }

    void ConsoleManager::PushMessage(const LogLevel level, const std::string &message)
    {
        if (m_Messages.size() > 1000)
            m_Messages.erase(m_Messages.begin());

        const std::string &timeStamp = GetCurrentTime();
        m_Messages.push_back({ message, timeStamp, level });
    }

    void ConsoleManager::Clear()
    {
        m_Messages.clear();
    }

    ConsoleManager &ConsoleManager::Get()
    {
        return *s_Instance;
    }

    void ConsoleManager::PushFormattedMessage(const LogLevel level, const std::string& message)
    {
        PushMessage(level, message);
    }

    std::vector<ConsoleMessage> &ConsoleManager::GetMessages()
    {
        return s_Instance->m_Messages;
    }

}