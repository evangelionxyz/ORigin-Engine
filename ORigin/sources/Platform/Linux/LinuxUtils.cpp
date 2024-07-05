#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Core/Application.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>

namespace origin
{
    std::string ExecCommand(const char* cmd)
    {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe)
        {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        {
            result += buffer.data();
        }
        // Remove the trailing newline character
        if (!result.empty() && result[result.length() - 1] == '\n') 
        {
            result.erase(result.length() - 1);
        }
        return result;
    }

    std::string FileDialogs::OpenFile(const char* filter)
    {
        std::string command = "zenity --file-selection";
        if (filter)
        {
            command += " --file-filter=\"";
            command += filter;
            command += "\"";
        }
        std::string result = ExecCommand(command.c_str());
        if (result.empty())
        {
            return {};
        }
        return result;
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        std::string command = "zenity --file-selection --save";
        if (filter)
        {
            command += " --file-filter=\"";
            command += filter;
            command += "\"";
        }
        std::string result = ExecCommand(command.c_str());
        if (result.empty())
        {
            return {};
        }
        return result;
    }
}
