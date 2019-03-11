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

#include <Windows.h>
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

#define NC_LOG_MESSAGE(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::Print(message, __VA_ARGS__);

#define NC_LOG_WARNING(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintWarning(message, __VA_ARGS__);

#define NC_LOG_DEPRECATED(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintDeprecated(message, __VA_ARGS__);

#define NC_LOG_ERROR(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintError(message, __VA_ARGS__);

#define NC_LOG_FATAL(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintFatal(message, __VA_ARGS__);

#define NC_LOG_SUCCESS(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintSuccess(message, __VA_ARGS__);

class DebugHandler
{
public:
    static bool isInitialized;
    static void Initialize();

    template <typename... Args>
    static void Print(std::string message, Args... args)
    {
        //PrintColor("[Message]: ", 15);
        PrintColor(message + "\n", 7, args...);
    }

    template <typename... Args>
    static void PrintWarning(std::string message, Args... args)
    {
        PrintColor("[Warning]: ", 14);
        PrintColor(message + "\n", 7, args...);
    }

    template <typename... Args>
    static void PrintDeprecated(std::string message, Args... args)
    {
        PrintColor("[Deprecated]: ", 14);
        PrintColor(message + "\n", 7, args...);
    }

    template <typename... Args>
    static void PrintError(std::string message, Args... args)
    {
        PrintColor("[Error]: ", 12);
        PrintColor(message + "\n", 7, args...);
    }

    template <typename... Args>
    static void PrintFatal(std::string message, Args... args)
    {
        PrintColor("[Fatal]: ", 12);
        PrintColor(message + "\n", 7, args...);
        assert(false);
    }

    template <typename... Args>
    static void PrintSuccess(std::string message, Args... args)
    {
        PrintColor("[Success]: ", 10);
        PrintColor(message + "\n", 7, args...);
    }

private:
    template <typename... Args>
    static void PrintColor(std::string message, u8 color, Args... args)
    {
        SetConsoleTextAttribute(_handle, color);
        printf(message.c_str(), args...);
        SetConsoleTextAttribute(_handle, _defaultColor);
    }

    static u32 _defaultColor;
    static HANDLE _handle;
};