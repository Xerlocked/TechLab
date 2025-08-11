#pragma once
#include "Editor/EditorWindow.h"
#include "Interface/ISwitchable.h"
#include "Interface/ICommand.h"

#include "../ImGui/imgui.h"
#include <ctype.h>

class URenderer;

class ConsoleWindow : public UEditorWindow, public ISwitchable, public ICommand
{
public:
	ConsoleWindow();
    ~ConsoleWindow() override;

	void Render() override;
    void OnResize(UINT32 Width, UINT32 Height) override;

    void Toggle() override;
    void Execute(const char*) override;

    static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static int   Strncmp(const char* s1, const char* s2, int n) { return n ? (*s1 == *s2 ? (*s1 ? Strncmp(s1 + 1, s2 + 1, n - 1) : 0) : ((unsigned char)*s1 - (unsigned char)*s2)) : 0; }
    static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ConsoleWindow* console = (ConsoleWindow*)data->UserData;
        return console->TextEditCallback(data);
    }

    int TextEditCallback(ImGuiInputTextCallbackData* data);

    void SetRenderer(URenderer* InRenderer) { MainRenderer = InRenderer; }
private:
    void AddLog(const char* fmt, ...);

    void ClearLog();

    void ExecCommand(const char* CommandLine);

private:
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    bool bWasOpen;

    URenderer* MainRenderer;
};

