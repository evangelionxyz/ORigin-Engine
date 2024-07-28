#include "pch.h"
#include "ConsoleManager.h"

namespace origin
{
    static ConsoleManager *s_Instance = nullptr;

    ConsoleManager::ConsoleManager()
    {
        s_Instance = this;
        m_Messages.reserve(500);
    }

    ConsoleManager::~ConsoleManager()
    {
        Clear();
    }

    std::string ConsoleManager::GetCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm local_tm = *std::localtime(&time_t_now);

        std::ostringstream oss;
        oss << std::put_time(&local_tm, "[%H:%M:%S]");
        return oss.str();
    }

    void ConsoleManager::PushMessage(ConsoleMessageType type, const std::string &message)
    {
        if (m_Messages.size() >= 500)
            m_Messages.erase(m_Messages.begin());

        std::string timeStamp = GetCurrentTime();
        m_Messages.push_back({ message, timeStamp, type });
    }

    void ConsoleManager::Clear()
    {
        m_Messages.clear();
    }

    ConsoleManager &ConsoleManager::Get()
    {
        return *s_Instance;
    }

    std::vector<ConsoleMessage> &ConsoleManager::GetMessages()
    {
        return s_Instance->m_Messages;
    }

}