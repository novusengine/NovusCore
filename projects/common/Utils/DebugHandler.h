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

#include <string>
#include <fstream>

#if defined(WIN32) && !defined(Release)
#include <Windows.h>
#endif

enum PROGRAM_TYPE
{
	Auth,
	Relay,
	World,
	Char
};

#ifndef Release
// Forward declaration
class DatabaseConnector; 

class DebugHandler
{
public:
	static void Init(PROGRAM_TYPE type);

	static void PrintMessage(std::string msg, const char* file, const char* func, size_t line);
	static void PrintDeprecated(std::string msg, const char* file, const char* func, size_t line);
	static void PrintWarning(std::string msg, const char* file, const char* func, size_t line);
	static void PrintError(std::string msg, const char* file, const char* func, size_t line);
	static void PrintFatal(std::string msg, const char* file, const char* func, size_t line);
private:

#ifdef WIN32
	static HANDLE hConsole;
#endif
	static size_t _debugLevel;
	
	static bool _logToDB;
	static std::unique_ptr<DatabaseConnector> _logDB;

	static bool _logToFile;
	static std::ofstream _logFile;
};
#endif

inline void InitDebugger(PROGRAM_TYPE type)
{
#ifndef Release
	DebugHandler::Init(type);
#endif
}

#ifndef Release
	#define NC_LOG_MESSAGE(msg) DebugHandler::PrintMessage(msg, __FILE__, __func__, __LINE__)
	#define NC_LOG_DEPRECATED(msg) DebugHandler::PrintDeprecated(msg, __FILE__, __func__, __LINE__)
	#define NC_LOG_WARNING(msg) DebugHandler::PrintWarning(msg, __FILE__, __func__, __LINE__)
	#define NC_LOG_ERROR(msg) DebugHandler::PrintError(msg, __FILE__, __func__, __LINE__)
	#define NC_LOG_FATAL(msg) DebugHandler::PrintFatal(msg, __FILE__, __func__, __LINE__)
#else
	#define NC_LOG_MESSAGE(msg) ((void)0)
	#define NC_LOG_DEPRECATED(msg) ((void)0)
	#define NC_LOG_WARNING(msg) ((void)0)
	#define NC_LOG_ERROR(msg) ((void)0)
	#define NC_LOG_FATAL(msg) ((void)0)
#endif