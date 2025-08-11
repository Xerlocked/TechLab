#include "OutlinerEditorPanel.h"
#include "World/World.h"
#include "GameFramework/Actor.h"
#include "Engine/EditorEngine.h"
#include "LevelEditor/SLevelEditor.h"
#include "Slate/Widgets/Layout/SSplitter.h"
#include <functional>

void OutlinerEditorPanel::Render()
{
    /* Pre Setup */
    ImGuiIO& io = ImGui::GetIO();
    
    // Splitter 기반 영역 계산
    SLevelEditor* LevelEditor = GEngineLoop.GetLevelEditor();
    FRect OutlinerRect{0,0,0,0};
    if (LevelEditor && LevelEditor->EditorHSplitter && LevelEditor->EditorHSplitter->SideLT)
    {
        OutlinerRect = LevelEditor->EditorHSplitter->SideLT->GetRect();
    }

    float PanelPosX = OutlinerRect.TopLeftX;
    float PanelPosY = OutlinerRect.TopLeftY;
    float PanelWidth = OutlinerRect.Width;
    float PanelHeight = OutlinerRect.Height;

    ImVec2 MinSize(140, 100);
    ImVec2 MaxSize(FLT_MAX, FLT_MAX);
    
    /* Min, Max Size */
    ImGui::SetNextWindowSizeConstraints(MinSize, MaxSize);
    
    /* Panel Position */
    ImGui::SetNextWindowPos(ImVec2(PanelPosX, PanelPosY), ImGuiCond_Always);

    /* Panel Size */
    ImGui::SetNextWindowSize(ImVec2(PanelWidth, PanelHeight), ImGuiCond_Always);

    /* Panel Flags */
    ImGuiWindowFlags PanelFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    
    /* Render Start */
    ImGui::Begin("Outliner", nullptr, PanelFlags);

    
    ImGui::BeginChild("Objects");
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        ImGui::EndChild();
        ImGui::End();

        return;
    }

    std::function<void(USceneComponent*)> CreateNode =
        [&CreateNode, &Engine](USceneComponent* InComp)->void
        {
            FString Name = InComp->GetName();

            ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_None;
            if (InComp->GetAttachChildren().Num() == 0)
            {
                Flags |= ImGuiTreeNodeFlags_Leaf;
            }
            
            ImGui::SetNextItemOpen(true, ImGuiCond_Always);
            bool NodeOpen = ImGui::TreeNodeEx(*Name, Flags);

            if (ImGui::IsItemClicked())
            {
                Engine->SelectActor(InComp->GetOwner());
                Engine->SelectComponent(InComp);
            }

            if (NodeOpen)
            {
                for (USceneComponent* Child : InComp->GetAttachChildren())
                {
                    CreateNode(Child);
                }
                ImGui::TreePop(); // 트리 닫기
            }
        };

    for (AActor* Actor : Engine->ActiveWorld->GetActiveLevel()->Actors)
    {
        ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_None;

        ImGui::SetNextItemOpen(true, ImGuiCond_Always);

        bool NodeOpen = ImGui::TreeNodeEx(*Actor->GetName(), Flags);

        if (ImGui::IsItemClicked())
        {
            Engine->SelectActor(Actor);
            Engine->DeselectComponent(Engine->GetSelectedComponent());
        }

        if (NodeOpen)
        {
            if (Actor->GetRootComponent())
            {
                CreateNode(Actor->GetRootComponent());
            }
                ImGui::TreePop();
        }
    }

    ImGui::EndChild();

    ImGui::End();
}
    
void OutlinerEditorPanel::OnResize(HWND hWnd)
{
    if (hWnd != Handle)
    {
        return;
    }
    
    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Width = static_cast<FLOAT>(ClientRect.right - ClientRect.left);
    Height = static_cast<FLOAT>(ClientRect.bottom - ClientRect.top);
}
