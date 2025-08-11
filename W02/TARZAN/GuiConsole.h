#pragma once
class GuiController;
class GuiConsole {
private:
	static char* Strdup(const char* s);
	static void Strtrim(char* s);
	static int Stricmp(const char* s1, const char* s2);
	static int Strnicmp(const char* s1, const char* s2, int n);
	static TArray<FString> StrSplit(FString s);
public:
	GuiConsole(GuiController*);
	void Render();
	void AddLog(const char* fmt, ...);
	void AddLog(const std::string str);
	void ClearLog();
	bool OnFocusing();
private:
	char _inputBuffer[256];
	ImVector<char*> _items;
	ImVector<char*> _commands;
	ImGuiTextFilter _filter;
	GuiController* _controller;
	void ExecCommand(const char* command_line);
	bool onFocus = false;
};

