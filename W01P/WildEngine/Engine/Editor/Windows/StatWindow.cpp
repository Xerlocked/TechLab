#include "StatWindow.h"
#include "../../../ImGui/imgui.h"
#include "Object/ObjectManager.h"

#include "Types/Types.h"
#include "Object/ObjectManager.h"

StatWindow::StatWindow()
{
    bWasOpen = true;
}

void StatWindow::Render()
{
    if (!bWasOpen)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    float scaleX = io.DisplaySize.x / 1024.0f;
    float scaleY = io.DisplaySize.y / 1024.0f;

    ImVec2 WinSize(io.DisplaySize.x * 0.2f, io.DisplaySize.y * 0.3f);

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.8f + 5, io.DisplaySize.y - WinSize.y - 5), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(WinSize.x - 5 * scaleX, WinSize.y), ImGuiCond_Once);


    ImGuiWindowFlags WindowFlags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Stat", &bWasOpen, WindowFlags))
    {
        ImGui::End(); // 반드시 호출해야 함
        return;
    }


    ImGui::Text(u8"Spawnned Objects: %d", UObjectManager::GetInst().GetTotalAllocationCount());
    
    ImGui::Text(u8"Memory Usage: %d bytes", UObjectManager::GetInst().GetTotalAllocationBytes());

    ImGui::SameLine(0, 5.0f);

    ImGui::End();
}

void StatWindow::OnResize(UINT32 Width, UINT32 Height)
{

}

void StatWindow::Toggle()
{
    bWasOpen = !bWasOpen;
}