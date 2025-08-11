#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdarg>
#include <stdio.h>

#include "ConsoleWindow.h"
#include "Types/Types.h"
#include "Editor/EditorDesigner.h"
#include "Object/ObjectFactory.h"
#include "Components/CubeComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TriangleComponent.h"

ConsoleWindow::ConsoleWindow()
{
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;

    // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    Commands.push_back("CLASSIFY");
    Commands.push_back("UE_LOG");
    Commands.push_back("stat");
    Commands.push_back("new");
    Commands.push_back("save");
    Commands.push_back("load");
    Commands.push_back("spawn");

    AutoScroll = true;
    ScrollToBottom = false;
    AddLog("Welcome to Wild Engine !");

    bWasOpen = true;
}

ConsoleWindow::~ConsoleWindow()
{
    ClearLog();
    for (int i = 0; i < History.Size; i++)
    {
        ImGui::MemFree(History[i]);
    }
}

void ConsoleWindow::Render()
{
    if (!bWasOpen)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    float scaleX = io.DisplaySize.x / 1024.0f;
    float scaleY = io.DisplaySize.y / 1024.0f;

    ImVec2 WinSize(io.DisplaySize.x * 0.8f, io.DisplaySize.y * 0.3f);

    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - WinSize.y - 5), ImGuiCond_Once);
    ImGui::SetNextWindowSize(WinSize, ImGuiCond_Once);

    ImGuiWindowFlags WindowFlags = 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Console Command", &bWasOpen, WindowFlags))
    {
        ImGui::End(); // 반드시 호출해야 함
        return;
    }


    ImGui::TextWrapped(
        "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
        "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    ImGui::TextWrapped("Enter 'HELP' for help.");

    // TODO: display items starting from the bottom

    if (ImGui::SmallButton("Add Debug Text")) { AddLog("%d some text", Items.Size); AddLog("some more text"); AddLog("display very important message here!"); }
    ImGui::SameLine();
    if (ImGui::SmallButton("Add Debug Error")) { AddLog("[error] something went wrong"); }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear")) { ClearLog(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Options, Filter
    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (const char* item : Items)
        {
            if (!Filter.PassFilter(item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
    {
        char* s = InputBuf;
        Strtrim(s);
        if (s[0])
            ExecCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void ConsoleWindow::OnResize(UINT32 Width, UINT32 Height)
{

}

void ConsoleWindow::Toggle()
{
    bWasOpen = !bWasOpen;
}

void ConsoleWindow::Execute(const char* command)
{
    ExecCommand(command);
}

int ConsoleWindow::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
        // Example of TEXT COMPLETION

        // Locate beginning of current word
        const char* word_end = data->Buf + data->CursorPos;
        const char* word_start = word_end;
        while (word_start > data->Buf)
        {
            const char c = word_start[-1];
            if (c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            word_start--;
        }

        // Build a list of candidates
        ImVector<const char*> candidates;
        for (int i = 0; i < Commands.Size; i++)
            if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
                candidates.push_back(Commands[i]);

        if (candidates.Size == 0)
        {
            // No match
            AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
        }
        else if (candidates.Size == 1)
        {
            // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
            data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
            data->InsertChars(data->CursorPos, candidates[0]);
            data->InsertChars(data->CursorPos, " ");
        }
        else
        {
            // Multiple matches. Complete as much as we can..
            // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
            int match_len = (int)(word_end - word_start);
            for (;;)
            {
                int c = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                    if (i == 0)
                        c = toupper(candidates[i][match_len]);
                    else if (c == 0 || c != toupper(candidates[i][match_len]))
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                match_len++;
            }

            if (match_len > 0)
            {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
            }

            // List matches
            AddLog("Possible matches:\n");
            for (int i = 0; i < candidates.Size; i++)
                AddLog("- %s\n", candidates[i]);
        }

        break;
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
        // Example of HISTORY
        const int prev_history_pos = HistoryPos;
        if (data->EventKey == ImGuiKey_UpArrow)
        {
            if (HistoryPos == -1)
                HistoryPos = History.Size - 1;
            else if (HistoryPos > 0)
                HistoryPos--;
        }
        else if (data->EventKey == ImGuiKey_DownArrow)
        {
            if (HistoryPos != -1)
                if (++HistoryPos >= History.Size)
                    HistoryPos = -1;
        }

        // A better implementation would preserve the data on the current input line along with cursor position.
        if (prev_history_pos != HistoryPos)
        {
            const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, history_str);
        }
    }
    }
    return 0;
}

void ConsoleWindow::AddLog(const char* fmt, ...)
{
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    Items.push_back(Strdup(buf));
}

void ConsoleWindow::ClearLog()
{
    for (int i = 0; i < Items.Size; i++)
        ImGui::MemFree(Items[i]);
    Items.clear();
}

void ConsoleWindow::ExecCommand(const char* CommandLine)
{
    AddLog("# %s\n", CommandLine);

    // Insert into history. First find match and delete it so it can be pushed to the back.
    // This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for (int i = History.Size - 1; i >= 0; i--)
        if (Stricmp(History[i], CommandLine) == 0)
        {
            ImGui::MemFree(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    History.push_back(Strdup(CommandLine));

    // Process command
    if (Stricmp(CommandLine, "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(CommandLine, "HELP") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < Commands.Size; i++)
            AddLog("- %s", Commands[i]);
    }
    else if (Stricmp(CommandLine, "HISTORY") == 0)
    {
        int first = History.Size - 10;
        for (int i = first > 0 ? first : 0; i < History.Size; i++)
            AddLog("%3d: %s\n", i, History[i]);
    }
    else if (Stricmp(CommandLine, "stat") == 0)
    {
        auto Window = UEditorDesigner::Get().GetWindow("StatWindow");
        if (Window)
        {
            // dynamic_cast를 통해 MyWindow 타입으로 변환 후 setter 호출
            if (ISwitchable* Stat = dynamic_cast<ISwitchable*>(Window.get()))
            {
                Stat->Toggle();
            }
        }
    }

    else if (Strnicmp(CommandLine, "spawn ", 6) == 0)
    {
        // "spawn " 이후 문자열 추출
        FString args = CommandLine + 6;
        // 첫 번째 공백을 찾아 shape와 count 분리
        size_t pos = args.find(' ');
        if (pos != FString::npos)
        {
            FString shape = args.substr(0, pos);
            FString countStr = args.substr(pos + 1);

            // 인자 앞뒤의 공백 제거 (필요시)
            while (!shape.empty() && isspace(shape.front()))
                shape.erase(shape.begin());
            while (!shape.empty() && isspace(shape.back()))
                shape.pop_back();
            while (!countStr.empty() && isspace(countStr.front()))
                countStr.erase(countStr.begin());
            while (!countStr.empty() && isspace(countStr.back()))
                countStr.pop_back();

            // shape가 cube, sphere, triangle 중 하나인지 확인
            if (shape == "cube" || shape == "sphere" || shape == "triangle")
            {
                int count = atoi(countStr.c_str());
                if (count > 0)
                {
                    // 실제 객체 생성을 위한 로직 호출 부분
                    AddLog("Spawning %d %s(s)...\n", count, shape.c_str());
                    // 예: SpawnShape(shape, count);

                    for (int i = 0; i < count; i++)
                    {
                        FVector pos(i * 2, 0, 0);
                        if (shape == "cube")
                        {
                            UCubeComponent* c = UObjectFactory::GetInst().ConstructObject<UCubeComponent>(UCubeComponent::GetClass(), MainRenderer);
                            c->SetRelativeLocation(pos);
                        }
                        else if (shape == "sphere")
                        {
                            USphereComponent* s = UObjectFactory::GetInst().ConstructObject<USphereComponent>(USphereComponent::GetClass(), MainRenderer);
                            s->SetRelativeLocation(pos);
                        }
                        else if (shape == "triangle")
                        {
                            UTriangleComponent* t = UObjectFactory::GetInst().ConstructObject<UTriangleComponent>(UTriangleComponent::GetClass(), MainRenderer);
                            t->SetRelativeLocation(pos);
                        }
                    }
                }
                else
                {
                    AddLog(u8"잘못된 개수입니다: %s\n", countStr.c_str());
                }
            }
            else
            {
                AddLog(u8"알 수 없는 객체 타입입니다: '%s'\n", shape.c_str());
            }
        }
        else
        {
            AddLog(u8"사용법: spawn <cube|sphere|triangle> <count>\n");
        }
    }

    else if (Strncmp(CommandLine, "UE_LOG(", 7) == 0)
    {
        // UE_LOG( 이후 문자열 추출
        FString args = CommandLine + 7;
        size_t endPos = args.find_last_of(')');
        if (endPos != FString::npos)
        {
            args = args.substr(0, endPos);
        }

        size_t commaPos = args.find(',');
        if (commaPos != FString::npos)
        {
            FString formatPart = args.substr(0, commaPos); // 포맷 부분
            FString argPart = args.substr(commaPos + 1);     // 인자 부분

            size_t firstQuote = formatPart.find_first_of('"'); // 앞 따옴표
            size_t lastQuote = formatPart.find_last_of('"');   // 뒷 따옴표

            if (firstQuote != FString::npos &&
                lastQuote != FString::npos &&
                firstQuote != lastQuote)
            {
                FString formatStr = formatPart.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                char buffer[1024];

                // %d, %f, %s 서식 지정자에 따른 변환 처리
                if (formatStr.find("%d") != FString::npos)
                {
                    int argValue = atoi(argPart.c_str());
                    snprintf(buffer, sizeof(buffer), formatStr.c_str(), argValue);
                }
                else if (formatStr.find("%f") != FString::npos)
                {
                    double argValue = atof(argPart.c_str());
                    snprintf(buffer, sizeof(buffer), formatStr.c_str(), argValue);
                }
                else if (formatStr.find("%s") != FString::npos)
                {
                    // 인자가 따옴표로 감싸져 있다면 제거
                    if (!argPart.empty() && argPart.front() == '"' && argPart.back() == '"')
                    {
                        argPart = argPart.substr(1, argPart.size() - 2);
                    }
                    snprintf(buffer, sizeof(buffer), formatStr.c_str(), argPart.c_str());
                }
                else
                {
                    // 서식 지정자가 없는 경우 기본적으로 인자를 정수로 처리
                    int argValue = atoi(argPart.c_str());
                    snprintf(buffer, sizeof(buffer), formatStr.c_str(), argValue);
                }

                AddLog("%s", buffer);
            }
            else
            {
                AddLog("FORMAT ERROR: '%s'\n", CommandLine);
            }
        }
        else
        {
            AddLog("ARGUMENT ERROR: '%s'\n", CommandLine);
        }
    }
    else
    {
        AddLog("Unknown command: '%s'\n", CommandLine);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}


