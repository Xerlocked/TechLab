#pragma once
class FDebugConsole
{
public:
	FDebugConsole();

	static void DebugPrint(const char* format, ...);
};

