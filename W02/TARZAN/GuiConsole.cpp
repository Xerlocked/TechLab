#include "stdafx.h"
#include "GuiConsole.h"
#include "UWorld.h"

int GuiConsole::Stricmp(const char* s1, const char* s2) { 
    int d; 
    while ( (d = toupper(*s2) - toupper(*s1)) == 0 && *s1 ) { 
        s1++; s2++; 
    } 
    return d;
}
int GuiConsole::Strnicmp(const char* s1, const char* s2, int n) { 
    int d = 0; 
    while ( n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1 ) { 
        s1++; s2++; n--; 
    } 
    return d; 
}

char* GuiConsole::Strdup(const char* s) { 
	IM_ASSERT(s); 
	size_t len = strlen(s) + 1; 
	void* buf = ImGui::MemAlloc(len); 
	IM_ASSERT(buf); 
	return (char*)memcpy(buf, (const void*)s, len); 
}

void GuiConsole::Strtrim(char* s) { 
	char* str_end = s + strlen(s); 
	while ( str_end > s && str_end[-1] == ' ' ) 
		str_end--; 
	*str_end = 0; 
}

TArray<FString> GuiConsole::StrSplit(FString s) {
    size_t pos_start = 0, pos_end, delim_len = 1;
    std::string token;
    std::vector<std::string> res;

    while ( (pos_end = s.find(" ", pos_start)) != std::string::npos ) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

GuiConsole::GuiConsole(GuiController* g): _controller(g) {}

void GuiConsole::Render() {
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    float consoleWindowWidth = static_cast<float>(SCR_WIDTH) * 1.f;
    float consoleWindowHeight = static_cast<float>(SCR_HEIGHT) * 0.2f;
    float consoleWindowPosX = (static_cast<float>(SCR_WIDTH) - consoleWindowWidth) * 0.5f;
    float consoleWindowPosY = (static_cast<float>(SCR_HEIGHT) - consoleWindowHeight) * 1.f;
    ImGui::SetNextWindowPos(ImVec2(consoleWindowPosX, consoleWindowPosY), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(consoleWindowWidth, consoleWindowHeight), ImGuiCond_Always);
	ImGui::Begin("Console");

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar);
	for (const char* item: _items) {
		//if ( _filter.PassFilter(item) )
		//	continue;
		ImGui::TextUnformatted(item);
	}

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

	ImGui::EndChild();
	ImGui::Separator(); 
    bool keepFocus = false;
    onFocus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll;
    if ( ImGui::InputText("Input", _inputBuffer, IM_ARRAYSIZE(_inputBuffer), input_text_flags, 0, (void*)this) ) {
        //OutputDebugString(L"console");
		char* s = _inputBuffer;
		Strtrim(s);
		if ( s[0] )
			ExecCommand(s);
		strcpy_s(s, 256, "");
        keepFocus = true;
	}
    if (keepFocus && onFocus)
        ImGui::SetKeyboardFocusHere(-1);
    else
        keepFocus = false;

	ImGui::End();
}

void GuiConsole::AddLog(const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	_items.push_back(Strdup(buf));
}

void GuiConsole::AddLog(const std::string str) {
	AddLog(str.c_str());
}

void GuiConsole::ClearLog() {
	for (auto& item: _items) {
		ImGui::MemFree(item);
	}
	_items.clear();
}

bool GuiConsole::OnFocusing()
{
    return onFocus;
}

void GuiConsole::ExecCommand(const char* command_line) {
    AddLog("# %s\n", command_line);
    FString str(command_line);
    TArray<FString> commands = StrSplit(str);
    if (commands[0] == "spawn") {
        UWorld* world = _controller->world;

        int count;
        if (commands.size() == 1) {
            AddLog("Invalid argument.");
            return;
        } else if ( commands.size() == 2 )
            count = 1;
        else if (commands.size() == 3)
        {
            if ( atoi(commands[2].c_str()) >= 0 || commands[2] == "0" )
                count = atoi(commands[2].c_str());
            else {
                AddLog("Invalid argument.");
                return;
            }
        }

        for (int i = 0; i < count; i++) {
            if ( commands[1] == "cube" ) {
                world->SpawnCubeActor();
            } else if ( commands[1] == "sphere" ) {
                world->SpawnSphereActor();
            } else if ( commands[1] == "plane" ) {
                world->SpawnPlaneActor();
            } else {
                AddLog("Invalid argument.");
                return;
            }
        }
        AddLog("Successed");

    } else if (commands[0] == "clear") {
        _items.clear();
    } else {
        AddLog("Invaild command.");
    }
}