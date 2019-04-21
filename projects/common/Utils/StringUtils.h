#pragma once

#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "../NovusTypes.h"

namespace StringUtils
{
    inline std::vector<std::string> SplitString(std::string string, char delim = ' ')
    {
        std::vector<std::string> results;
        std::stringstream ss(string);
        std::string token;

        while (std::getline(ss, token, delim)) {
            results.push_back(token);
        }

        return results;
    }

    inline std::string EscapeString(std::string const& string)
    {
        std::size_t n = string.length();
        std::string escaped;
        escaped.reserve(n * 2);        // pessimistic preallocation

        for (std::size_t i = 0; i < n; ++i) {
            if (string[i] == '\\' || string[i] == '\'')
                escaped += '\\';
            escaped += string[i];
        }
        return escaped;
    }

    template <typename... Args>
    inline i32 FormatString(char* buffer, size_t bufferSize, char const* format, Args... args)
    {
        i32 length = -1;
#ifdef _WIN32
        length = sprintf_s(buffer, bufferSize, format, args...);
#else
        (void)bufferSize; // Get rid of warning
        length = sprintf(buffer, format, args...);
#endif
        assert(length > -1);
        return length;
    }

    // FNV-1a 32bit hashing algorithm.
    constexpr u32 fnv1a_32(char const* s, std::size_t count)
    {
        return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
    }
}

constexpr u32 operator"" _h(char const* s, std::size_t count)
{
    return StringUtils::fnv1a_32(s, count);
}