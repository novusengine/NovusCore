#include "DebugHandler.h"

bool DebugHandler::isInitialized = false;
u32 DebugHandler::_defaultColor = NULL;
#ifdef _WIN32
HANDLE DebugHandler::_handle = NULL;
#endif

void DebugHandler::Initialize()
{
    #ifdef _WIN32
    _handle = GetStdHandle(STD_OUTPUT_HANDLE); // MAYBE this instead: GetConsoleWindow();
    CONSOLE_SCREEN_BUFFER_INFO info;

    if (!GetConsoleScreenBufferInfo(_handle, &info))
    {
        assert(false);
    }


    _defaultColor = info.wAttributes;
    #endif
    isInitialized = true;
}