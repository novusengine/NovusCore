#include "DebugHandler.h"

bool DebugHandler::isInitialized = false;
u32 DebugHandler::_defaultColor = NULL;
HANDLE DebugHandler::_handle = NULL;

void DebugHandler::Initialize()
{
    _handle = GetStdHandle(STD_OUTPUT_HANDLE); // MAYBE this instead: GetConsoleWindow();
    CONSOLE_SCREEN_BUFFER_INFO info;

    if (!GetConsoleScreenBufferInfo(_handle, &info))
    {
        assert(false);
    }

    _defaultColor = info.wAttributes;
    isInitialized = true;
}