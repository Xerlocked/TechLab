#include "ControlWindow.h"
#include "../../../ImGui/imgui.h"
#include "../../../ImGui/imgui_internal.h"

#include "Editor/EditorDesigner.h"
#include "Components/GizmoComponent.h"
#include "Interface/ICommand.h"

ControlWindow::ControlWindow()
{
	FramePerSecond = 0;
	
	PrimitiveTypeNumber = 0;
	SpawnNumber = 0;
	SceneName = "NewScene";
	bIsOrthogonal = nullptr;

	FOV = nullptr;
	CameraLocation = nullptr;
	CameraRotation = nullptr;

    Location[0] = 0;
    Location[1] = 0;
    Location[2] = 0;


    Rotation[0] = 0;
    Rotation[1] = 0;
    Rotation[2] = 0;

    WindowWidth = 360;
    WindowHeight = 320;
}

void ControlWindow::Render()
{
    ImGuiIO& io = ImGui::GetIO();

    float scaleX = io.DisplaySize.x / 1024.0f;
    float scaleY = io.DisplaySize.y / 1024.0f;

    ImVec2 WinSize(WindowWidth * scaleX, WindowHeight * scaleY);

    ImGui::SetNextWindowPos(ImVec2(5, 10), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(WinSize, ImGuiCond_Appearing);

    // ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Always);

	ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoResize);

	ImGui::Text("WILD ENGINE - KRFTON JUNGLE");
	ImGui::Text("FPS %.0f (%.0f ms)", io.Framerate, 1000.0f / io.Framerate);

    ImGui::Separator(); // 수평 구분선
    // 컨트롤 버튼 섹션

    ImFont* UnicodeFont = io.Fonts->Fonts[FEATHER_FONT];

    ImVec2 ControlButtonSize = ImVec2(32, 32);

    ImGui::PushFont(UnicodeFont);
    ImVec4 ActiveColor = ImVec4(0, 0.5, 0, 0.6f);

    bool isTranslationActive = (PrimaryGizmo && PrimaryGizmo->GetCurrentGizmo() == EGizmoType::Translation);
    if (isTranslationActive)
        ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor); // 활성 상태 색상
    if (ImGui::Button(u8"\ue9bc", ControlButtonSize))
    {
        if (PrimaryGizmo)
        {
            PrimaryGizmo->SetGizmoType(EGizmoType::Translation);
        }
    }
    if (isTranslationActive)
        ImGui::PopStyleColor();

    ImGui::SameLine(0, 5.0f);

    // Rotation 버튼 (활성 상태이면 색상을 변경)
    bool isRotationActive = (PrimaryGizmo && PrimaryGizmo->GetCurrentGizmo() == EGizmoType::Rotation);
    if (isRotationActive)
        ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor);
    if (ImGui::Button(u8"\ue9d3", ControlButtonSize))
    {
        if (PrimaryGizmo)
        {
            PrimaryGizmo->SetGizmoType(EGizmoType::Rotation);
        }
    }
    if (isRotationActive)
        ImGui::PopStyleColor();

    ImGui::SameLine(0, 5.0f);

    // Scale 버튼 (활성 상태이면 색상을 변경)
    bool isScaleActive = (PrimaryGizmo && PrimaryGizmo->GetCurrentGizmo() == EGizmoType::Scale);
    if (isScaleActive)
        ImGui::PushStyleColor(ImGuiCol_Button, ActiveColor);
    if (ImGui::Button(u8"\ue9ab", ControlButtonSize))
    {
        if (PrimaryGizmo)
        {
            PrimaryGizmo->SetGizmoType(EGizmoType::Scale);
        }
    }
    if (isScaleActive)
        ImGui::PopStyleColor();

    ImGui::SameLine();
    // 창 내부의 전체 콘텐츠 영역 너비를 가져옵니다.
    float windowContentWidth = ImGui::GetWindowContentRegionMax().x;

    // 두 버튼과 버튼 사이 간격의 총 너비를 계산합니다.
    float buttonsTotalWidth = ControlButtonSize.x * 2 + 5.0f;

    // 오른쪽 정렬을 위해 버튼들이 시작될 X 좌표를 계산합니다.
    float posX = windowContentWidth - buttonsTotalWidth;
    if (posX < 0)
        posX = 0;

    // 현재 커서 위치의 Y는 그대로 유지하고, X 좌표만 오른쪽으로 이동합니다.
    ImGui::SetCursorPosX(posX);

    if (ImGui::Button(u8"\ue9b7", ControlButtonSize))
    {
        auto Window = UEditorDesigner::Get().GetWindow("ConsoleWindow");
        if (Window)
        {
            // dynamic_cast를 통해 MyWindow 타입으로 변환 후 setter 호출
            if (ISwitchable* Console = dynamic_cast<ISwitchable*>(Window.get()))
            {
                Console->Toggle();
            }
        }
    }

    ImGui::SameLine(0, 5.0f);

    if (ImGui::Button(u8"\ue918", ControlButtonSize))
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

    ImGui::PopFont();

    // 컨트롤 버튼 섹션
	ImGui::Separator(); // 수평 구분선
    // 스폰 섹션

	const char* items[] = { "cube", "sphere", "triangle" };
    ImGui::SetNextItemWidth(100); // 원하는 너비
	ImGui::Combo("Primitive", &PrimitiveTypeNumber, items, IM_ARRAYSIZE(items));
    
    ImGui::SameLine(0, 5.0f);

    if (CreateCustomInputInt("Number Of Spawn", ImGuiDataType_S32, &SpawnNumber, "%d", ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsDecimal))
    {
        FString TargetPrimitiveStr = items[PrimitiveTypeNumber];
        auto Window = UEditorDesigner::Get().GetWindow("ConsoleWindow");
        if (Window)
        {
            // dynamic_cast를 통해 MyWindow 타입으로 변환 후 setter 호출
            if (ICommand* Console = dynamic_cast<ICommand*>(Window.get()))
            {
                FString Count = std::to_string(SpawnNumber);
                FString CommandStr = "spawn " + TargetPrimitiveStr + " " + Count;
                Console->Execute(CommandStr.c_str());
            }
        }
    }

    // 스폰 섹션
    ImGui::Separator(); // 수평 구분선
    // 씬 섹션

    ImGui::PushFont(UnicodeFont);
    
    NewScene = ImGui::Button(u8"\ue96d", ControlButtonSize); // New Scene

    ImGui::SameLine(0, 5.0f);
    LoadScene = ImGui::Button(u8"\ue950", ControlButtonSize); // Load Scene

    ImGui::SameLine(0, 5.0f);
    SaveScene = ImGui::Button(u8"\ue9d6", ControlButtonSize); // Save Scene

    
    ImGui::PopFont();

    char buf[20];
    strncpy_s(buf, SceneName.c_str(), sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';

    ImGui::SetNextItemWidth(150); // 원하는 너비

    if (ImGui::InputText("SceneName", buf, sizeof(buf)))
    {
        SceneName = buf;
    }

    // 씬 섹션
    ImGui::Separator(); // 수평 구분선

    if (ImGui::Checkbox("Orthogonal", bIsOrthogonal))
    {

    }

    if (ImGui::DragFloat("FOV", FOV, 1, 0, 120))
    {

    }

    UpdateVectorToFloat(CameraLocation, Location);
    if (ImGui::DragFloat3("Camera Location", Location))
    {
        CameraLocation->X = Location[0];
        CameraLocation->Y = Location[1];
        CameraLocation->Z = Location[2];
    }

    UpdateVectorToFloat(CameraRotation, Rotation);
    if (ImGui::DragFloat3("Camera Rotation", Rotation))
    {
        CameraRotation->X = Rotation[0];
        CameraRotation->Y = Rotation[1];
        CameraRotation->Z = Rotation[2];
    }

	ImGui::End();
}

void ControlWindow::OnResize(UINT32 Width, UINT32 Height)
{
}

void ControlWindow::SetPrimaryGizmo(UGizmoComponent* NewGizmo)
{
    if(PrimaryGizmo == NewGizmo) return;

    PrimaryGizmo = NewGizmo;
}

bool ControlWindow::CreateCustomInputInt(const char* label, ImGuiDataType data_type, void* p_data, const char* format, ImGuiInputTextFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    IM_ASSERT((flags & ImGuiInputTextFlags_EnterReturnsTrue) == 0); // Not supported by InputScalar()!

    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    void* p_data_default = (g.NextItemData.HasFlags & ImGuiNextItemDataFlags_HasRefVal) ? &g.NextItemData.RefVal : &g.DataTypeZeroValue;

    char buf[64];
    if ((flags & ImGuiInputTextFlags_DisplayEmptyRefVal) && ImGui::DataTypeCompare(data_type, p_data, p_data_default) == 0)
        buf[0] = 0;
    else
        ImGui::DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, p_data, format);

    // Disable automatic MarkItemEdited(), we'll call it manually.
    g.NextItemData.ItemFlags |= ImGuiItemFlags_NoMarkEdited;
    flags |= ImGuiInputTextFlags_AutoSelectAll | (ImGuiInputTextFlags)ImGuiInputTextFlags_LocalizeDecimalPoint;

    bool value_changed = false;
    const float button_size = ImGui::GetFrameHeight();

    ImGui::BeginGroup(); // 그룹으로 묶어 외부에서 아이템 정보를 얻을 수 있게 함.
    ImGui::PushID(label);
    ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 6 + 15));
    if (ImGui::InputText("", buf, IM_ARRAYSIZE(buf), flags))
        ImGui::DataTypeApplyFromText(buf, data_type, p_data, format, (flags & ImGuiInputTextFlags_ParseEmptyRefVal) ? p_data_default : NULL);
    IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Inputable);

    // 기존의 Step 버튼(-, +) 부분을 제거하고, 대신 "spawn" 버튼을 추가합니다.
    const ImVec2 backup_frame_padding = style.FramePadding;
    style.FramePadding.x = style.FramePadding.y;
    if (flags & ImGuiInputTextFlags_ReadOnly)
        ImGui::BeginDisabled();
    ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
    ImGui::SameLine(0, style.ItemInnerSpacing.x);
    if (ImGui::ButtonEx("Spawn", ImVec2(button_size * 4.2f, button_size)))
    {
        // "spawn" 버튼 클릭 시 원하는 동작을 수행합니다.
        // 예를 들어, 특별한 연산을 적용하거나 상태를 변경할 수 있습니다.
        value_changed = true;
    }
    ImGui::PopItemFlag();
    if (flags & ImGuiInputTextFlags_ReadOnly)
        ImGui::EndDisabled();

    style.FramePadding = backup_frame_padding;

    ImGui::PopID();
    ImGui::EndGroup();
        

    g.LastItemData.ItemFlags &= ~ImGuiItemFlags_NoMarkEdited;
    if (value_changed)
        ImGui::MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

void ControlWindow::UpdateVectorToFloat(FVector* v, float f[])
{
    f[0] = v->X;
    f[1] = v->Y;
    f[2] = v->Z;
}