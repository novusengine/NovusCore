/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif
#include <string>
#include <cassert>
#include "../NovusTypes.h"

enum PROGRAM_TYPE
{
    Auth,
    Relay,
    Char,
    World
};

enum ColorCode
{
    GREEN = 10,
    YELLOW = 14,
    MAGENTA = 5,
    RED = 12,
};

#define NC_LOG_MESSAGE(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::Print(message, ##__VA_ARGS__);

#define NC_LOG_WARNING(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintWarning(message, ##__VA_ARGS__);

#define NC_LOG_DEPRECATED(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintDeprecated(message, ##__VA_ARGS__);

#define NC_LOG_ERROR(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintError(message, ##__VA_ARGS__);

#define NC_LOG_FATAL(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintFatal(message, ##__VA_ARGS__);

#define NC_LOG_SUCCESS(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintSuccess(message, ##__VA_ARGS__);

class DebugHandler
{
public:
    static bool isInitialized;
    static void Initialize();

    template <typename... Args>
    inline static void Print(std::string message, Args... args)
    {
        printf((message + "\n").c_str(), args...);
    }

    template <typename... Args>
    inline static void PrintWarning(std::string message, Args... args)
    {
        PrintColor("[Warning]: ", ColorCode::YELLOW);
        Print(message, args...);
    }

    template <typename... Args>
    inline static void PrintDeprecated(std::string message, Args... args)
    {
        PrintColor("[Deprecated]: ", ColorCode::YELLOW);
        Print(message, args...);
    }

    template <typename... Args>
    inline static void PrintError(std::string message, Args... args)
    {
        PrintColor("[Error]: ", ColorCode::MAGENTA);
        Print(message, args...);
    }

    template <typename... Args>
    inline static void PrintFatal(std::string message, Args... args)
    {
        PrintColor("[Fatal]: ", ColorCode::RED);
        Print(message, args...);
        assert(false);
    }

    template <typename... Args>
    inline static void PrintSuccess(std::string message, Args... args)
    {
        PrintColor("[Success]: ", ColorCode::GREEN);
        Print(message, args...);
    }
private:
    template <typename... Args>
    inline static void PrintColor(std::string message, ColorCode color, Args...args)
    {
#ifdef _WIN32
        SetConsoleTextAttribute(_handle, color);
        printf(message.c_str(), args...);
        SetConsoleTextAttribute(_handle, _defaultColor);
#else
        const std::string green("\033[1;32m");
        const std::string yellow("\033[1;33m");
        const std::string magenta("\033[0;35m");
        const std::string red("\033[0;31m");
        const std::string reset("\033[0m");

        std::string withColors;
        switch(color)
        {
            case ColorCode::RED:
                withColors = red + message + reset;
                break;
            case ColorCode::YELLOW:
                withColors = yellow + message + reset;
                break;
            case ColorCode::MAGENTA:
                withColors = magenta + message + reset;
                break;
            case ColorCode::GREEN:
                withColors = green + message + reset;
                break;
            default:
                withColors = message;
        }

        printf(withColors.c_str(), args...);
#endif
    }

#ifdef _WIN32
    static u32 _defaultColor;
    static HANDLE _handle;
#endif
};
