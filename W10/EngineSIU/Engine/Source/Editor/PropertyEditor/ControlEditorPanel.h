#pragma once
#include "Components/ActorComponent.h"
#include "UnrealEd/EditorPanel.h"
#include "LightGridGenerator.h"

class ControlEditorPanel : public UEditorPanel
{
public:
    virtual void Render() override;
    virtual void OnResize(HWND hWnd) override;

private:
    void CreateMenuButton(ImVec2 ButtonSize, ImFont* IconFont);
    void CreateModifyButton(ImVec2 ButtonSize, ImFont* IconFont);
    static void CreateFlagButton();
    void CreatePIEButton(ImVec2 ButtonSize, ImFont* IconFont) const;
    static void CreateSRTButton(ImVec2 ButtonSize);
    void CreateLightSpawnButton(ImVec2 InButtonSize, ImFont* IconFont);
    
private:
    float Width = 1400, Height = 1000;
    bool bOpenModal = false;
    bool bShowImGuiDemoWindow = false; // 데모 창 표시 여부를 관리하는 변수
    bool bShowSkeletalMeshViewer = false; // 스켈레탈 메시 뷰어 표시 여부를 관리하는 변수
    bool bShowAnimationViewer = false; // Animation viewer control boolean.

    float* FOV = nullptr;
    float CameraSpeed = 0.0f;
    float GridScale = 1.0f;
    FLightGridGenerator LightGridGenerator;
};

