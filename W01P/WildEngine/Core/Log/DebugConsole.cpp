#include "DebugConsole.h"
#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include "Types/Types.h"

FDebugConsole::FDebugConsole()
{
}

void FDebugConsole::DebugPrint(const char* format, ...)
{
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsprintf_s(buffer, format, args);
    va_end(args);

    FString output(buffer);
    output += "\n";

    OutputDebugStringA(output.c_str());
}
