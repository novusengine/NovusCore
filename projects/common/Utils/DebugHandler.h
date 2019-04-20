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

#define NC_LOG_MESSAGE(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::Print(message);

#define NC_LOG_WARNING(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintWarning(message);

#define NC_LOG_DEPRECATED(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintDeprecated(message);

#define NC_LOG_ERROR(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintError(message);

#define NC_LOG_FATAL(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintFatal(message);

#define NC_LOG_SUCCESS(message, ...) if (!DebugHandler::isInitialized) { DebugHandler::Initialize(); } \
DebugHandler::PrintSuccess(message);

class DebugHandler
{
public:
    static bool isInitialized;
    static void Initialize();

    inline static void Print(std::string message)
    {
        //PrintColor("[Message]: ", 15);
		//PrintColor(message + "\n", 7);
		printf((message + "\n").c_str());
	}

	inline static void PrintWarning(std::string message)
    {
        PrintColor("[Warning]: ", 14);
        PrintColor(message + "\n", 7);
    }

	inline static void PrintDeprecated(std::string message)
    {
        PrintColor("[Deprecated]: ", 14);
        PrintColor(message + "\n", 7);
    }

	inline static void PrintError(std::string message)
    {
        PrintColor("[Error]: ", 12);
        PrintColor(message + "\n", 7);
    }

	inline static void PrintFatal(std::string message)
    {
        PrintColor("[Fatal]: ", 12);
        PrintColor(message + "\n", 7);
        assert(false);
    }

	inline static void PrintSuccess(std::string message)
    {
        PrintColor("[Success]: ", 10);
        PrintColor(message + "\n", 7);
    }

private:
	inline static void PrintColor(std::string message, u8 color)
    {
        #ifdef _WIN32
        SetConsoleTextAttribute(_handle, color);
        #endif
        printf(message.c_str());
        #ifdef _WIN32
        SetConsoleTextAttribute(_handle, _defaultColor);
        #endif
    }

    static u32 _defaultColor;
    #ifdef _WIN32
    static HANDLE _handle;
    #endif
};