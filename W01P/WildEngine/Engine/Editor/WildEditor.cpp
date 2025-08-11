#include "WildEditor.h"

#include <fstream>

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../imGui/imgui_impl_win32.h"

#include "Renderer/URenderer.h"
#include "Scene/Scene.h"
#include "Components/CameraComponent.h"

#include "EditorDesigner.h"
#include "Editor/Windows/ControlWindow.h"
#include "Editor/Windows/PropertyWindow.h"
#include "Editor/Windows/ConsoleWindow.h"
#include "Editor/Windows/StatWindow.h"

#include "Font/IconDefs.h"
#include "Font/RawFonts.h"

#include "json.hpp"
#include "Object/ObjectManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CubeComponent.h"
#include "Components/TriangleComponent.h"
#include "Components/SphereComponent.h"

UWildEditor::UWildEditor(URenderer* InRenderer)
{
    Renderer = InRenderer;
}

UWildEditor::UWildEditor(const UWildEditor&)
{
}

UWildEditor::~UWildEditor()
{
    Renderer = nullptr;
}

void UWildEditor::Create(ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext, HWND InHWND)
{
    if (Device == nullptr)
    {
        Device = InDevice;
    }

    if (DeviceContext == nullptr)
    {
        DeviceContext = InDeviceContext;
    }

    if (hWnd == nullptr)
    {
        hWnd = InHWND;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init((void*)hWnd);
    ImGui_ImplDX11_Init(InDevice, InDeviceContext);

    CreateUsingFont();
    
    auto NewControlWindow = std::make_shared<ControlWindow>();
    UEditorDesigner::Get().AddWindow("ControlWindow", NewControlWindow);

    auto NewPropertyWindow = std::make_shared<PropertyWindow>();
    UEditorDesigner::Get().AddWindow("PropertyWindow", NewPropertyWindow);

    auto NewConsoleWindow = std::make_shared<ConsoleWindow>();
    UEditorDesigner::Get().AddWindow("ConsoleWindow", NewConsoleWindow);

    auto NewStatWindow = std::make_shared<StatWindow>();
    UEditorDesigner::Get().AddWindow("StatWindow", NewStatWindow);
}

void UWildEditor::Release()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UWildEditor::Render()
{
    PreferenceStyle();

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Scene = Renderer->GetPrimaryScene();

    SetupControlWindow();
    SetupPropertyWindow();
    SetupConsoleWindow();

    UEditorDesigner::Get().Render();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UWildEditor::OnResize()
{
    Release();
    while (UEditorDesigner::Get().IsClear())
    {
        Create(Device, DeviceContext, hWnd);
        UEditorDesigner::Get().Clear();
    }
}

void UWildEditor::PreferenceStyle()
{
    // Window
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
    ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImGui::GetStyle().WindowRounding = 5.0f;

    ImGui::GetStyle().FrameRounding = 3.0f;

    // Sep
    ImGui::GetStyle().Colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    
    // Frame
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.31f, 0.31f, 0.31f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.203f, 0.203f, 0.203f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

    // Button
    ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

    ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(0.203f, 0.203f, 0.203f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

    // Text
    ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 0.9f);
    
}

void UWildEditor::CreateUsingFont()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesKorean());

    ImFontConfig FeatherFontConfig;
    FeatherFontConfig.PixelSnapH = true;
    FeatherFontConfig.FontDataOwnedByAtlas = false;
    FeatherFontConfig.GlyphOffset = ImVec2(0, 0);
    static const ImWchar IconRanges[] = { 
        ICON_MOVE,      ICON_MOVE + 1, 
        ICON_ROTATE,    ICON_ROTATE + 1, 
        ICON_SCALE,     ICON_SCALE + 1, 
        ICON_MONITOR,   ICON_MONITOR + 1,
        ICON_BAR_GRAPH, ICON_BAR_GRAPH +1,
        ICON_NEW,       ICON_NEW + 1,
        ICON_SAVE,      ICON_SAVE + 1,
        ICON_LOAD,      ICON_LOAD + 1,
        0 };

    io.Fonts->AddFontFromMemoryTTF(FeatherRawData, FontSizeOfFeather, 22.0f, &FeatherFontConfig, IconRanges);
}

void UWildEditor::SetupControlWindow()
{
    auto Window = UEditorDesigner::Get().GetWindow("ControlWindow");
    if (Window)
    {
        if (ControlWindow* Control = dynamic_cast<ControlWindow*>(Window.get()))
        {
            Control->SetPrimaryGizmo(Scene->GetGizmo());
            Control->SetCameraLocation(Scene->GetPrimaryCamera()->CameraPosition);
            Control->SetCameraRotation(Scene->GetPrimaryCamera()->CameraRotation);
            Control->SetCameraFOV(Scene->GetPrimaryCamera()->FieldOfView);
            Control->SetOrthogonal(Scene->GetPrimaryCamera()->bIsOrthogonal);
            FString SceneName = Control->GetSceneName();
            if (Control->NewScene) {
				NewScene(SceneName);
            }
			if (Control->LoadScene) {
				LoadScene(SceneName);
			}
            if (Control->SaveScene) {
				SaveScene(SceneName);
            }
        }
    }
}

void UWildEditor::NewScene(FString SceneName)
{
    TArray<UObject*>& GUObjectArray = UObjectManager::GetInst().GetObjectsArray();
    for (int32 i = static_cast<int32>(GUObjectArray.size()) - 1; i >= 0; i--)
    {
        if (GUObjectArray[i]->IsA(UCubeComponent::GetClass()) || GUObjectArray[i]->IsA(USphereComponent::GetClass()) || GUObjectArray[i]->IsA(UTriangleComponent::GetClass()))
        {
            delete GUObjectArray[i];
        }
    }
    Scene->SetSelectedObject(nullptr);
}

void UWildEditor::LoadScene(FString SceneName)
{
    std::ifstream inFile(SceneName + ".Scene");
    if (inFile.is_open())
    {
        TArray<UObject*>& GUObjectArray = UObjectManager::GetInst().GetObjectsArray();
        for (int32 i = static_cast<int32>(GUObjectArray.size()) - 1; i >= 0; i--)
        {
            if (GUObjectArray[i]->IsA(UCubeComponent::GetClass()) || GUObjectArray[i]->IsA(USphereComponent::GetClass()) || GUObjectArray[i]->IsA(UTriangleComponent::GetClass()))
            {
                delete GUObjectArray[i];
            }
        }
        Scene->SetSelectedObject(nullptr);

        FString jsonData;
        inFile.seekg(0, std::ios::end);
        jsonData.reserve(inFile.tellg());
        inFile.seekg(0, std::ios::beg);
        jsonData.assign((std::istreambuf_iterator<char>(inFile)),
            std::istreambuf_iterator<char>());
        json::JSON Scene = json::JSON::Load(jsonData);
        uint32 Version = Scene["Version"].ToInt();
        uint32 NextUUID = Scene["NextUUID"].ToInt();
        json::JSON Primitives = Scene["Primitives"];

        for (auto it = Primitives.ObjectRange().begin(); it != Primitives.ObjectRange().end(); ++it)
        {
            uint32 UUID = std::stoi(it->first);
            json::JSON Primitive = it->second;
            FString Type = Primitive["Type"].ToString();
            FVector Location = json::JSONToFVector(Primitive["Location"]);
            FVector Rotation = json::JSONToFVector(Primitive["Rotation"]);
            FVector Scale = json::JSONToFVector(Primitive["Scale"]);
            if (!Type.compare("Sphere"))
            {
                USphereComponent* Sphere = new USphereComponent(Renderer);
                Sphere->UUID = UUID;
                Sphere->RelativeLocation = Location;
                Sphere->RelativeRotation = Rotation;
                Sphere->RelativeScale3D = Scale;
            }
            else if (!Type.compare("Cube"))
            {
                UCubeComponent* Cube = new UCubeComponent(Renderer);
                Cube->UUID = UUID;
                Cube->RelativeLocation = Location;
                Cube->RelativeRotation = Rotation;
                Cube->RelativeScale3D = Scale;
            }
            else if (!Type.compare("Triangle"))
            {
                UTriangleComponent* Triangle = new UTriangleComponent(Renderer);
                Triangle->UUID = UUID;
                Triangle->RelativeLocation = Location;
                Triangle->RelativeRotation = Rotation;
                Triangle->RelativeScale3D = Scale;
            }
        }

    }
    else {
    }
}


void UWildEditor::SetupPropertyWindow()
{
    auto Window = UEditorDesigner::Get().GetWindow("PropertyWindow");
    if (Window)
    {
        if (PropertyWindow* Property = dynamic_cast<PropertyWindow*>(Window.get()))
        {
            if (Scene->GetSelectedObject() != nullptr) {
                Property->SetLocation(Scene->GetSelectedObject()->RelativeLocation);
                Property->SetRotation(Scene->GetSelectedObject()->RelativeRotation);
                Property->SetScale(Scene->GetSelectedObject()->RelativeScale3D);

                Property->SetUUID(Scene->GetSelectedObject()->UUID);

                Property->SetFocusObject(true);
            }
            else
            {
                Property->SetFocusObject(false);
            }
        }
    }
}

void UWildEditor::SetupConsoleWindow()
{
    auto Window = UEditorDesigner::Get().GetWindow("ConsoleWindow");
    if (Window)
    {
        if (ConsoleWindow* Console = dynamic_cast<ConsoleWindow*>(Window.get()))
        {
            Console->SetRenderer(Scene->GetRenderer());
        }
    }
}

void UWildEditor::SaveScene(FString SceneName)
{
    uint32 Version = 1;
    uint32 NextUUID = UObjectManager::GetInst().GetNextUUID();
    TArray<UObject*>& GUObjectArray = UObjectManager::GetInst().GetObjectsArray();

    json::JSON Scene;
    Scene["Version"] = Version;
    Scene["NextUUID"] = NextUUID;

    // GUObjectArray를 순회하면서 Primitive만 저장
    for (uint32 i = 0; i < GUObjectArray.size(); i++)
    {
        if (GUObjectArray[i]->IsA(UCubeComponent::GetClass()) || GUObjectArray[i]->IsA(USphereComponent::GetClass()) || GUObjectArray[i]->IsA(UTriangleComponent::GetClass()))
        {
            UPrimitiveComponent* Primitive = static_cast<UPrimitiveComponent*>(GUObjectArray[i]);
            FString key = std::to_string(GUObjectArray[i]->UUID);
            Scene["Primitives"][key]["Location"] = json::FVectorToJSON(Primitive->RelativeLocation);
            Scene["Primitives"][key]["Rotation"] = json::FVectorToJSON(Primitive->RelativeRotation);
            Scene["Primitives"][key]["Scale"] = json::FVectorToJSON(Primitive->RelativeScale3D);
            FString RawTypeName = Primitive->GetInstanceClass()->ClassName;
            Scene["Primitives"][key]["Type"] = CleanTypeName(RawTypeName);
        }
    }

    FString jsonData = Scene.dump();
    std::ofstream outFile(SceneName + ".Scene");
    if (outFile.is_open())
    {
        outFile << jsonData;
        outFile.close();
    }
    else {
    }
}


//String 분해
FString UWildEditor::CleanTypeName(const FString& rawName)
{
    FString result = rawName;

    // "U"분리
    const FString prefix = "U";
    if (result.find(prefix) == 0)
    {
        result = result.substr(prefix.length());
    }

    // "Component" 분리
    const FString suffix = "Component";
    size_t pos = result.rfind(suffix);
    if (pos != FString::npos)
    {
        result = result.substr(0, pos);
    }

    return result;
}