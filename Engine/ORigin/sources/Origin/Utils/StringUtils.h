// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <sstream>

namespace origin::Utils
{
	std::string BytesToString(uint64_t bytes);
	std::string CapitalizeText(std::string& text);
	std::string CapitalizeWholeText(std::string text);

    static void FormatStringImpl(std::ostringstream &ss, const char *format)
    {
        ss << format;
    }

    template<typename T, typename... Args>
    static void FormatStringImpl(std::ostringstream &ss, const char *format, T &&value, Args&&... args)
    {
        while (*format)
        {
            if (*format == '{' && *(format + 1) == '}')
            {
                ss << value;
                FormatStringImpl(ss, format + 1, std::forward<Args>(args)...);
                return;
            }

            ss << *format++;
        }
    }

    template<typename... Args>
    static std::string FormatString(const char *format, Args&&... args)
    {
        std::ostringstream ss;
        FormatStringImpl(ss, format, std::forward<Args>(args)...);
        return ss.str();
    }

    
}

#endif