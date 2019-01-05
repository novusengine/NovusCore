#include "DebugHandler.h"

#ifndef Release
#include <iostream>
#include "../Config/ConfigHandler.h"
//#include "../Database/DatabaseConnector.h"

#ifdef WIN32
#define SetTextColor(NUMBER) SetConsoleTextAttribute(hConsole, NUMBER)
#else
#define SetTextColor(NUMBER) ((void)0)
#endif

#ifdef WIN32
HANDLE DebugHandler::hConsole = NULL;
#endif
bool DebugHandler::_logToDB = false;
//std::unique_ptr<DatabaseConnector> DebugHandler::_logDB = nullptr;
bool DebugHandler::_logToFile = false;
std::ofstream DebugHandler::_logFile;
size_t DebugHandler::_debugLevel = 1;


void DebugHandler::Init(PROGRAM_TYPE type)
{
#ifdef WIN32
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	_logToDB = false;
	_logToFile = false;
	_debugLevel = 3;

	// TODO: Enable this when ConfigHandler is working
	/*if (!ConfigHandler::IsInitialized())
		NC_LOG_FATAL("ConfigHandler has to be initialized before DebugHandler!");
	
	_logToDB = ConfigHandler::GetOption<bool>("LogToDB", false);
	_logToFile = ConfigHandler::GetOption<bool>("LogToFile", false);
	_debugLevel = ConfigHandler::GetOption<size_t>("DebugLevel", 1);

	if (_logToFile)
	{
		std::string logFilePath = ConfigHandler::GetOption<std::string>("LogFilePath", "ERROR");
		if (logFilePath == "ERROR")
			_logToFile = false;
		else
			_logFile.open(logFilePath);
	}*/
}

void DebugHandler::PrintMessage(std::string msg, const char* file, const char* func, size_t line)
{
	if (_debugLevel < 3)
		return;

	std::cout << "[MSG]: " << msg << std::endl;

	if (_logToFile)
		_logFile << "[MSG]: " << msg << std::endl;
}

void DebugHandler::PrintDeprecated(std::string msg, const char* file, const char* func, size_t line)
{
	if (_debugLevel < 2)
		return;

	SetTextColor(8); // Gray
	std::cout << "[DEPRECATED]: ";
	SetTextColor(7); // Default white
	std::cout << msg << " - " << file << ": " << line << std::endl;

	if (_logToFile)
		_logFile << "[DEPRECATED]: " << msg << " - " << file << ": " << line << std::endl;
}

void DebugHandler::PrintWarning(std::string msg, const char* file, const char* func, size_t line)
{
	if (_debugLevel < 2)
		return;

	SetTextColor(14); // Yellow
	std::cout << "[WARNING]: ";
	SetTextColor(7); // Default white
	std::cout << msg << " - " << file << ": " << line << std::endl;

	if (_logToFile)
		_logFile << "[WARNING]: " << msg << " - " << file << ": " << line << std::endl;
}

void DebugHandler::PrintError(std::string msg, const char* file, const char* func, size_t line)
{
	if (_debugLevel < 1)
		return;

	SetTextColor(4); // Red
	std::cout << "[ERROR]: ";
	SetTextColor(7); // Default white
	std::cout << msg << " - " << file << ": " << line << std::endl;

	if (_logToFile)
		_logFile << "[ERROR]: " << msg << " - " << file << ": " << line << std::endl;
}

void DebugHandler::PrintFatal(std::string msg, const char* file, const char* func, size_t line)
{
	SetTextColor(4); // Red
	std::cerr << "[FATAL ERROR]: ";
	SetTextColor(7); // Default white
	std::cerr << msg << " - " << file << ": " << line << std::endl;

	if (_logToFile)
	{
		_logFile << "[FATAL ERROR]: " << msg << " - " << file << ": " << line << std::endl;
		_logFile.close();
	}

	abort();
}

#endif