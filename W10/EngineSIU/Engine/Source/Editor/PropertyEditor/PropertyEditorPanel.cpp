#include "PropertyEditorPanel.h"

#include <algorithm>
#include <string>
//#include <windows.h>
//#include <tchar.h>

#include "World/World.h"
#include "Actors/Player.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Light/LightComponent.h"
#include "Components/Light/PointLightComponent.h"
#include "Components/Light/SpotLightComponent.h"
#include "Components/Light/DirectionalLightComponent.h"
#include "Components/Light/AmbientLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextComponent.h"
#include "Engine/EditorEngine.h"
#include "Engine/FObjLoader.h"
#include "UnrealEd/ImGuiWidget.h"
#include "UObject/ObjectFactory.h"
#include "Engine/Engine.h"
#include "Components/HeightFogComponent.h"
#include "Components/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/FbxObject.h"
#include "Engine/FbxManager.h"
#include "Engine/Asset/SkeletalMeshAsset.h"
#include "Components/Mesh/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Viewer/SlateViewer.h"
#include "Slate/Widgets/Layout/SSplitter.h"
#include "Components/Material/Material.h"
#include "Contents/LuaAnimInstance.h"
#include "Contents/MyAnimInstance.h"
#include "Contents/Actors/ItemActor.h"
#include "Math/JungleMath.h"
#include "Renderer/ShadowManager.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UObject/UObjectIterator.h"
#include "LuaScripts/LuaScriptComponent.h"
#include "LuaScripts/LuaScriptFileUtils.h"
#include "imgui/imgui_bezier.h"
#include "imgui/imgui_curve.h"
#include <Animation/AnimNode_State.h>
#include "Animation/AnimationStateMachine.h"
#include <Contents/PreviewAnimInstance.h>

#define USE_UPROPERTY_IMGUI false

void PropertyEditorPanel::Render()
{
    UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
    if (!Engine)
    {
        return;
    }

    /* Pre Setup */
    // Splitter 기반 영역 계산
    FRect PropertyRect{ 0,0,0,0 };

    SLevelEditor* LevelEditor = GEngineLoop.GetLevelEditor();
    if (LevelEditor && LevelEditor->EditorHSplitter && LevelEditor->EditorHSplitter->SideRB)
    {
        PropertyRect = LevelEditor->EditorHSplitter->SideRB->GetRect();
    }
    else
    {
        PropertyRect = FRect{ 0, 0, static_cast<float>(Width), static_cast<float>(Height) };
    }
    
    float PanelPosX = PropertyRect.TopLeftX;
    float PanelPosY = PropertyRect.TopLeftY;
    float PanelWidth = PropertyRect.Width;
    float PanelHeight = PropertyRect.Height;

    ImVec2 MinSize(140, 370);
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
    ImGui::Begin("Detail", nullptr, PanelFlags);

    AActor* SelectedActor = Engine->GetSelectedActor();
    USceneComponent* SelectedComponent = Engine->GetSelectedComponent();
    USceneComponent* TargetComponent = nullptr;

    if (SelectedComponent != nullptr)
    {
        TargetComponent = SelectedComponent;
    }
    else if (SelectedActor != nullptr)
    {
        TargetComponent = SelectedActor->GetRootComponent();
    }

    if (TargetComponent != nullptr)
    {
        AEditorPlayer* Player = Engine->GetEditorPlayer();
        RenderForSceneComponent(TargetComponent, Player);
    }

    if (SelectedActor)
    {
        RenderForActor(SelectedActor, TargetComponent);
    }

    if (UAmbientLightComponent* LightComponent = GetTargetComponent<UAmbientLightComponent>(SelectedActor, SelectedComponent))
    {
        RenderForAmbientLightComponent(LightComponent);
    }
    if (UDirectionalLightComponent* LightComponent = GetTargetComponent<UDirectionalLightComponent>(SelectedActor, SelectedComponent))
    {
        RenderForDirectionalLightComponent(LightComponent);
    }
    if (UPointLightComponent* LightComponent = GetTargetComponent<UPointLightComponent>(SelectedActor, SelectedComponent))
    {
        RenderForPointLightComponent(LightComponent);
    }
    if (USpotLightComponent* LightComponent = GetTargetComponent<USpotLightComponent>(SelectedActor, SelectedComponent))
    {
        RenderForSpotLightComponent(LightComponent);
    }
    if (UProjectileMovementComponent* ProjectileComp = GetTargetComponent<UProjectileMovementComponent>(SelectedActor, SelectedComponent))
    {
        RenderForProjectileMovementComponent(ProjectileComp);
    }
    if (UTextComponent* TextComp = GetTargetComponent<UTextComponent>(SelectedActor, SelectedComponent))
    {
        RenderForTextComponent(TextComp);
    }
    if (UStaticMeshComponent* StaticMeshComponent = GetTargetComponent<UStaticMeshComponent>(SelectedActor, SelectedComponent))
    {
        RenderForStaticMesh(StaticMeshComponent);
        RenderForMaterial(StaticMeshComponent);
    }
    if (USkeletalMeshComponent* SkeletalMeshComponent = GetTargetComponent<USkeletalMeshComponent>(SelectedActor, SelectedComponent))
    {
        RenderForSkeletalMesh(SkeletalMeshComponent);
        //RenderForModifySkeletalBone(SkeletalMeshComponent);
    }
    if (UHeightFogComponent* FogComponent = GetTargetComponent<UHeightFogComponent>(SelectedActor, SelectedComponent))
    {
        RenderForExponentialHeightFogComponent(FogComponent);
    }

    if (UCameraComponent* CameraComponent = GetTargetComponent<UCameraComponent>(SelectedActor, SelectedComponent))
    {
        RenderForCameraComponent(CameraComponent);
    }

    if (UShapeComponent* ShapeComponent = GetTargetComponent<UShapeComponent>(SelectedActor, SelectedComponent))
    {
        RenderForShapeComponent(ShapeComponent);
    }

    if (USpringArmComponent* SpringArmComponent = GetTargetComponent<USpringArmComponent>(SelectedActor, SelectedComponent))
    {
        RenderForSpringArmComponent(SpringArmComponent);
    }

    ImGui::End();
}

void PropertyEditorPanel::RGBToHSV(const float R, const float G, const float B, float& H, float& S, float& V)
{
    const float MX = FMath::Max(R, FMath::Max(G, B));
    const float MN = FMath::Min(R, FMath::Min(G, B));
    const float Delta = MX - MN;

    V = MX;

    if (MX == 0.0f) {
        S = 0.0f;
        H = 0.0f;
        return;
    }
    else {
        S = Delta / MX;
    }

    if (Delta < 1e-6) {
        H = 0.0f;
    }
    else {
        if (R >= MX) {
            H = (G - B) / Delta;
        }
        else if (G >= MX) {
            H = 2.0f + (B - R) / Delta;
        }
        else {
            H = 4.0f + (R - G) / Delta;
        }
        H *= 60.0f;
        if (H < 0.0f) {
            H += 360.0f;
        }
    }
}

void PropertyEditorPanel::HSVToRGB(const float H, const float S, const float V, float& R, float& G, float& B)
{
    // h: 0~360, s:0~1, v:0~1
    const float C = V * S;
    const float Hp = H / 60.0f;             // 0~6 구간
    const float X = C * (1.0f - fabsf(fmodf(Hp, 2.0f) - 1.0f));
    const float M = V - C;

    if (Hp < 1.0f) { R = C;  G = X;  B = 0.0f; }
    else if (Hp < 2.0f) { R = X;  G = C;  B = 0.0f; }
    else if (Hp < 3.0f) { R = 0.0f; G = C;  B = X; }
    else if (Hp < 4.0f) { R = 0.0f; G = X;  B = C; }
    else if (Hp < 5.0f) { R = X;  G = 0.0f; B = C; }
    else { R = C;  G = 0.0f; B = X; }

    R += M;  G += M;  B += M;
}

void PropertyEditorPanel::RenderForSceneComponent(USceneComponent* SceneComponent, AEditorPlayer* Player) 
{
    ImGui::SetItemDefaultFocus();
    // TreeNode 배경색을 변경 (기본 상태)
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        RenderProperties(SceneComponent);

        std::string CoordiButtonLabel;
        if (Player->GetCoordMode() == ECoordMode::CDM_WORLD)
        {
            CoordiButtonLabel = "World";
        }
        else if (Player->GetCoordMode() == ECoordMode::CDM_LOCAL)
        {
            CoordiButtonLabel = "Local";
        }

        if (ImGui::Button(CoordiButtonLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().IndentSpacing, 32)))
        {
            Player->AddCoordiMode();
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForCameraComponent(UCameraComponent* InCameraComponent)
{
}

void PropertyEditorPanel::RenderForPlayerActor(APlayer* InPlayerActor)
{
    if (ImGui::Button("SetMainPlayer"))
    {
        GEngine->ActiveWorld->SetMainPlayer(InPlayerActor);
    }
}

void PropertyEditorPanel::RenderForActor(AActor* SelectedActor, USceneComponent* TargetComponent) const
{
    if (ImGui::Button("Duplicate"))
    {
        UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
        AActor* NewActor = Engine->ActiveWorld->DuplicateActor(Engine->GetSelectedActor());
        Engine->SelectActor(NewActor);
        Engine->DeselectComponent(Engine->GetSelectedComponent());
    }

    FString BasePath = FString(L"LuaScripts\\");
    FString LuaDisplayPath;

    if (SelectedActor->GetComponentByClass<ULuaScriptComponent>())
    {
        LuaDisplayPath = SelectedActor->GetComponentByClass<ULuaScriptComponent>()->GetDisplayName();
        if (ImGui::Button("Edit Script"))
        {
            // 예: PickedActor에서 스크립트 경로를 받아옴
            if (auto* ScriptComp = SelectedActor->GetComponentByClass<ULuaScriptComponent>())
            {
                std::wstring ws = (BasePath + ScriptComp->GetDisplayName()).ToWideString();
                LuaScriptFileUtils::OpenLuaScriptFile(ws.c_str());
            }
        }
    }
    else
    {
        // Add Lua Script
        if (ImGui::Button("Create Script"))
        {
            // Lua Script Component 생성 및 추가
            ULuaScriptComponent* NewScript = SelectedActor->AddComponent<ULuaScriptComponent>();
            FString LuaFilePath = NewScript->GetScriptPath();
            std::filesystem::path FilePath = std::filesystem::path(GetData(LuaFilePath));

            try
            {
                std::filesystem::path Dir = FilePath.parent_path();
                if (!std::filesystem::exists(Dir))
                {
                    std::filesystem::create_directories(Dir);
                }

                std::ifstream luaTemplateFile(TemplateFilePath.ToWideString());

                std::ofstream file(FilePath);
                if (file.is_open())
                {
                    if (luaTemplateFile.is_open())
                    {
                        file << luaTemplateFile.rdbuf();
                    }
                    // 생성 완료
                    file.close();
                }
                else
                {
                    MessageBoxA(nullptr, "Failed to Create Script File for writing: ", "Error", MB_OK | MB_ICONERROR);
                }
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                MessageBoxA(nullptr, "Failed to Create Script File for writing: ", "Error", MB_OK | MB_ICONERROR);
            }
            LuaDisplayPath = NewScript->GetDisplayName();
        }
    }
    ImGui::InputText("Script File", GetData(LuaDisplayPath), IM_ARRAYSIZE(*LuaDisplayPath),
        ImGuiInputTextFlags_ReadOnly);

    if (ImGui::TreeNodeEx("Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("Add");
        ImGui::SameLine();

        TArray<UClass*> CompClasses;
        GetChildOfClass(USceneComponent::StaticClass(), CompClasses);

        if (ImGui::BeginCombo("##AddComponent", "Components", ImGuiComboFlags_None))
        {
            for (UClass* Class : CompClasses)
            {
                if (ImGui::Selectable(GetData(Class->GetName()), false))
                {
                    USceneComponent* NewComp = Cast<USceneComponent>(SelectedActor->AddComponent(Class));
                    if (NewComp != nullptr && TargetComponent != nullptr)
                    {
                        NewComp->SetupAttachment(TargetComponent);
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
}

void PropertyEditorPanel::RenderForStaticMesh(UStaticMeshComponent* StaticMeshComp) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Static Mesh", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("StaticMesh");
        ImGui::SameLine();

        FString PreviewName = FString("None");
        if (UStaticMesh* StaticMesh = StaticMeshComp->GetStaticMesh())
        {
            const FStaticMeshRenderData& RenderData = StaticMesh->GetRenderData();
            PreviewName = RenderData.DisplayName;
        }

        const TMap<FName, FAssetInfo> Assets = UAssetManager::Get().GetAssetRegistry();

        if (ImGui::BeginCombo("##StaticMesh", GetData(PreviewName), ImGuiComboFlags_None))
        {
            for (const auto& Asset : Assets)
            {
                if (Asset.Value.AssetType == EAssetType::StaticMesh)
                {
                    FString Label = Asset.Value.AssetName.ToString();

                    bool bIsLoaded = (Asset.Value.State == FAssetInfo::LoadState::Completed);
                    bool bIsFailed = (Asset.Value.State == FAssetInfo::LoadState::Failed);
                    bool bIsLoading = (Asset.Value.State == FAssetInfo::LoadState::Loading);

                    // 상태별로 이름 뒤에 태그 추가
                    if (bIsLoading) Label += TEXT(" (Loading)");
                    else if (bIsFailed) Label += TEXT(" (Failed)");

                    if (!bIsLoaded)
                        ImGui::BeginDisabled(); // 선택 불가하게 만듦

                    if (ImGui::Selectable(*Label, false) && bIsLoaded)
                    {
                        FString MeshName = Asset.Value.PackagePath.ToString() + "/" + Asset.Value.AssetName.ToString();
                        UStaticMesh* StaticMesh = FObjManager::GetStaticMesh(MeshName);
                        if (StaticMesh)
                        {
                            StaticMeshComp->SetStaticMesh(StaticMesh);
                        }
                    }

                    if (!bIsLoaded)
                        ImGui::EndDisabled();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void PropertyEditorPanel::DrawAnimationControls(USkeletalMeshComponent* SkeletalComp) 
{
    USkeletalMeshComponent* SelectedSkeleton = Cast<USkeletalMeshComponent>(SkeletalComp); // 선택된 스켈레탈 메시 컴포넌트 저장
    ImGui::Separator();
    ImGui::Text("Animation Control");
    ImGui::Spacing();

    if (!SelectedSkeleton) // SelectedSkeleton이 유효한지 먼저 확인
    {
        ImGui::Text("No Skeletal Mesh Component selected.");
        ImGui::Spacing();
        ImGui::Separator();
        return;
    }

    TArray<FString> animNames;
    {
        FSpinLockGuard Lock(FFbxManager::AnimMapLock);
        for (auto const& [name, entry] : FFbxManager::GetAnimSequences())
        {
            if (entry.State == FFbxManager::LoadState::Completed && entry.Sequence != nullptr)
            {
                animNames.Add(name);
            }
        }
    }

    static std::vector<std::string> AnimNameAnsiList;
    static std::vector<const char*> AnimNameAnsiPtrs;

    AnimNameAnsiList.clear();
    AnimNameAnsiPtrs.clear();

    for (const auto& Name : animNames) {
        AnimNameAnsiList.emplace_back(Name.ToAnsiString());
        AnimNameAnsiPtrs.push_back(AnimNameAnsiList.back().c_str());
    }

    const char* preview_value = (SelectedAnimIndex >= 0 && SelectedAnimIndex < AnimNameAnsiPtrs.size()) ? AnimNameAnsiPtrs[SelectedAnimIndex] : "None";

    if (ImGui::BeginCombo("Animations", preview_value))
    {
        for (int i = 0; i < animNames.Num(); ++i)
        {
            const bool is_selected = (SelectedAnimIndex == i);
            if (ImGui::Selectable(AnimNameAnsiPtrs[i], is_selected))
            {
                SelectedAnimIndex = i;
                SelectedAnimName = animNames[i]; // 선택된 애니메이션 이름 업데이트
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();

    bool bCanPlay = (SelectedAnimIndex != -1 && !SelectedAnimName.IsEmpty());

    if (ImGui::Button("Play Animation", ImVec2(120, 0)))
    {
        if (SelectedSkeleton && bCanPlay) // SelectedSkeleton 유효성 재확인
        {
            SelectedSkeleton->SetAnimationMode(EAnimationMode::AnimationSingleNode);
            UAnimSequence* animToPlay = FFbxManager::GetAnimSequenceByName(SelectedAnimName);
            //UAnimSequenceBase* animToPlay = FFbxManager::GetAnimSequenceByName(SelectedAnimName);
            if (animToPlay)
            {
                UE_LOG(ELogLevel::Display, TEXT("Playing animation: %s"), *SelectedAnimName);
                SelectedSkeleton->PlayAnimation(animToPlay, true); // bLooping = true
            }
            else
            {
                UE_LOG(ELogLevel::Warning, TEXT("Could not find or load animation: %s"), *SelectedAnimName);
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop Animation", ImVec2(120, 0)))
    {
        if (SelectedSkeleton)
        {
            UE_LOG(ELogLevel::Display, TEXT("Stopping animation."));
            SelectedSkeleton->PlayAnimation(nullptr, false); // null 재생으로 중지
            SelectedSkeleton->ResetPose(); // 기본 포즈로
        }
    }

    if (SelectedSkeleton)
    {
        UAnimSingleNodeInstance* SingleNodeInstance = SelectedSkeleton->GetSingleNodeInstance();
        if (SingleNodeInstance && SingleNodeInstance->IsPlaying())
        {
            UAnimSequenceBase* CurrentAnim = SingleNodeInstance->GetCurrentSequence();
            if (CurrentAnim)
            {
                float CurrentRate = CurrentAnim->GetRateScale();
                if (ImGui::SliderFloat("Rate Scale", &CurrentRate, -5.0f, 5.0f, "%.1f"))
                {
                    CurrentAnim->SetRateScale(CurrentRate);
                }
            }
        }
    }
    ImGui::Spacing();
    ImGui::Separator();
}

// TODO : StateMachine UI 수정 중
//void PropertyEditorPanel::DrawStateMachineDebugControls(USkeletalMeshComponent* SkeletalComp)
//{
//    UAnimInstance* AnimInst = SkeletalComp->GetAnimationInstance();
//    if (!AnimInst || !AnimInst->GetAnimSM()) return;
//    UAnimationStateMachine* SM = AnimInst->GetAnimSM();
//    
//    ImGui::Separator();
//    if (!ImGui::TreeNodeEx("StateMachine Editor", ImGuiTreeNodeFlags_Framed)) return;
//
//    // 트랜지션 및 리스트 노드 준비
//    // -- Nodes --
//    TArray<UAnimNode_State*> NodesRef = SM->GetStates();
//    // -- Transitions--
//    TArray<FAnimTransition> TransitionsRef = SM->GetTransitions();
//    
//    TArray<UAnimNode_State*> Nodes = SM->GetStates();
//    
//    if (ImGui::TreeNodeEx("States", ImGuiTreeNodeFlags_DefaultOpen)) {
//        for (int i = 0; i < NodesRef.Num(); ++i) {
//            UAnimNode_State* Node = NodesRef[i];
//            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
//                | ImGuiTreeNodeFlags_SpanAvailWidth
//                | ((SM->GetCurrentState() == Node->GetStateName()) ? ImGuiTreeNodeFlags_Selected : 0);
//            if (ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "State %d: %s", i, Node->GetStateNameFName().ToString())) {
//                ImGui::Text("Anim : %s", *Node->GetLinkAnimationSequence()->GetName());
//
//                static int animIdx = 0;
//                static std::vector<UAnimSequenceBase*> seqList= 
//            }
//        }
//    }
//    for (auto& T : SM->GetTransitions()) {
//        Nodes.AddUnique(T.FromState);
//        Nodes.AddUnique(T.ToState);
//    }
//
//    for (auto* Node : Nodes) {
//        ImGui::Text("State: %s", *Node->GetLinkAnimationSequence()->GetName());
//    }
//
//    static char newStateName[64] = "";
//    ImGui::InputText("New State Name", newStateName, IM_ARRAYSIZE(newStateName));
//    // 시퀀스 선택 콤보박스 ( 간단 예시 )
//    static int seqIdx = -1;
//    static TArray<UAnimSequenceBase*> seqList;
//    TArray<std::string> seqNames;
//    for (auto S : seqList) {
//        seqNames.Add(S->GetName().ToAnsiString());
//    }
//    /*if (ImGui::Combo("Link Sequence", &seqIdx, seqNames.GetData()->ToAnsiString(), seqNames.Num())) {}
//    if (ImGui::Button("Add State") && seqIdx >= 0) {
//        UAnimNode_State* NewNode = FObjectFactory::ConstructObject<UAnimNode_State>(AnimInst);
//        NewNode->Initialize(FName(newStateName), seqList[seqIdx]);
//        strcpy_s(newStateName, "");
//    }*/
//
//}
void PropertyEditorPanel::RenderForSkeletalMesh(USkeletalMeshComponent*SkeletalComp)
{
    DrawAnimationControls(SkeletalComp);
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Skinned Mesh", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("SkinnedMesh");
        ImGui::SameLine();

        FString PreviewName = FString("None");
        if (USkeletalMesh* SkeletalMesh = SkeletalComp->GetSkeletalMesh())
        {
            PreviewName = SkeletalMesh->GetObjectName();
        }

        const TMap<FName, FAssetInfo> Assets = UAssetManager::Get().GetAssetRegistry();

        if (ImGui::BeginCombo("##SkeletalMesh", GetData(PreviewName), ImGuiComboFlags_None))
        {
            for (const auto& Asset : Assets)
            {
                if (Asset.Value.AssetType == EAssetType::SkeletalMesh)
                {
                    FString Label = Asset.Value.AssetName.ToString();

                    bool bIsLoaded = (Asset.Value.State == FAssetInfo::LoadState::Completed);
                    bool bIsLoading = (Asset.Value.State == FAssetInfo::LoadState::Loading);
                    bool bIsFailed = (Asset.Value.State == FAssetInfo::LoadState::Failed);

                    // 상태에 따라 라벨에 태그 추가
                    if (bIsLoading)
                        Label += TEXT(" (Loading)");
                    else if (bIsFailed)
                        Label += TEXT(" (Failed)");

                    if (!bIsLoaded)
                        ImGui::BeginDisabled();  // 선택 비활성화

                    if (ImGui::Selectable(*Label, false) && bIsLoaded)
                    {
                        FString MeshName = Asset.Value.GetFullPath();
                        USkeletalMesh* SkeletalMesh = FFbxManager::GetSkeletalMesh(MeshName.ToWideString());
                        if (SkeletalMesh)
                        {
                            SkeletalComp->SetSkeletalMesh(SkeletalMesh);
                        }
                    }

                    if (!bIsLoaded)
                        ImGui::EndDisabled();
                }
            }
            ImGui::EndCombo();
        }

        static int SelectedAnimInstanceIndex = -1;
        static std::string SelectedAnimInstanceLabel = "None";

        TArray<UClass*> AnimClasses;
        GetChildOfClass(UAnimInstance::StaticClass(), AnimClasses);
        
        UAnimInstance* CurrentInstance = SkeletalComp->GetAnimationInstance();
        FString CurrentName = CurrentInstance ? CurrentInstance->GetClass()->GetName() : TEXT("None");
        const char* ComboLabel = !CurrentName.IsEmpty() ? *CurrentName : "None";

        if (ImGui::BeginCombo("AnimInstance", ComboLabel, ImGuiComboFlags_None))
        {
            for (auto* AnimInstance : AnimClasses)
            {
                FString UnrealName = AnimInstance ? AnimInstance->GetName() : TEXT("None");
                const char* ItemName = !UnrealName.IsEmpty() ? *UnrealName : "None";
                bool bIsSelected = (CurrentInstance && (AnimInstance == CurrentInstance->GetClass()));

                if (ImGui::Selectable(ItemName, bIsSelected))
                {
                    UAnimInstance* Instance = Cast<UAnimInstance>(FObjectFactory::ConstructObject(AnimInstance, GEngine));
                    SkeletalComp->SetAnimationInstance(Instance);

                }
                if (bIsSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        FString LuaDisplayPath;
        if (SkeletalComp->GetAnimationInstance())
        {
            FString BasePath = FString(L"LuaScripts\\");
            LuaDisplayPath = SkeletalComp->GetAnimationInstance()->GetDisplayName();

            if (Cast<ULuaAnimInstance>(SkeletalComp->GetAnimationInstance()))
            {
                FString LuaFilePath = SkeletalComp->GetAnimationInstance()->GetScriptPath();
                if (ImGui::Button("Edit Lua"))
                {
                    std::wstring ws = (BasePath + LuaDisplayPath).ToWideString();
                    LuaScriptFileUtils::OpenLuaScriptFile(ws.c_str());
                }
            }
            else
            {
                if (ImGui::Button("Create Lua"))
                {
                    FString LuaFilePath = SkeletalComp->GetAnimationInstance()->GetScriptPath();
                    std::filesystem::path FilePath = std::filesystem::path(GetData(LuaFilePath));


                    try
                    {
                        std::filesystem::path Dir = FilePath.parent_path();
                        if (!std::filesystem::exists(Dir))
                        {
                            std::filesystem::create_directories(Dir);
                        }

                        std::ifstream luaTemplateFile("LuaScripts/template_asm.lua");

                        std::ofstream file(FilePath);
                        if (file.is_open())
                        {
                            if (luaTemplateFile.is_open())
                            {
                                file << luaTemplateFile.rdbuf();
                            }
                            // 생성 완료
                            file.close();
                        }
                        else
                        {
                            MessageBoxA(nullptr, "Failed to Create Script File for writing: ", "Error", MB_OK | MB_ICONERROR);
                        }
                    }
                    catch (const std::filesystem::filesystem_error& e)
                    {
                        MessageBoxA(nullptr, "Failed to Create Script File for writing: ", "Error", MB_OK | MB_ICONERROR);
                    }
                }
            }
            
            static char LuaScriptTextBuffer[128] = { 0 };
    
            std::string NotifyNameStr = GetData(LuaDisplayPath);
            strncpy_s(LuaScriptTextBuffer, NotifyNameStr.c_str(), sizeof(LuaScriptTextBuffer));
            LuaScriptTextBuffer[sizeof(LuaScriptTextBuffer) - 1] = '\0'; // null-termination 보장
    
            if (ImGui::InputText("Script File", LuaScriptTextBuffer, IM_ARRAYSIZE(LuaScriptTextBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                LuaDisplayPath = LuaScriptTextBuffer;
            }
        }
        
        
        TArray<FString> animNames;
        {
            FSpinLockGuard Lock(FFbxManager::AnimMapLock);
            for (auto const& [name, entry] : FFbxManager::GetAnimSequences())
            {
                if (entry.State == FFbxManager::LoadState::Completed && entry.Sequence != nullptr)
                {
                    animNames.Add(name);
                }
            }
        }

        static int SelectedAnim1Index = -1;
        static std::string SelectedAnim1Label = "NONE";

        if (SelectedAnim1Index >= 0 && SelectedAnim1Index < animNames.Num())
        {
            SelectedAnim1Label = animNames[SelectedAnim1Index].ToAnsiString();
        }

        if (ImGui::BeginCombo("Idle - Anim1", SelectedAnim1Label.c_str()))
        {
            for (int i = 0; i < animNames.Num(); ++i)
            {
                if (ImGui::Selectable(*animNames[i], false))
                {
                    SelectedAnim1Index = i;
                    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(SkeletalComp->GetAnimationInstance());
                    UPreviewAnimInstance* PreviewAnimInstance = Cast<UPreviewAnimInstance>(SkeletalComp->GetAnimationInstance());
                    if (MyAnimInstance)
                    {
                        MyAnimInstance->Anim1 = FFbxManager::GetAnimSequenceByName(animNames[i]);
                        MyAnimInstance->SetCurrentSequence(MyAnimInstance->Anim1, 0.f);
                    }
                    else if (PreviewAnimInstance) {
                    

                        PreviewAnimInstance->Anim1 = FFbxManager::GetAnimSequenceByName(animNames[i]);
                        PreviewAnimInstance->SetCurrentSequence(PreviewAnimInstance->Anim1, 0.f);

                        uint32 StateKey = FName("Idle").GetComparisonIndex();

                        if (UAnimNode_State** IdleState = PreviewAnimInstance->GetAnimSM()->StateContainer.Find(StateKey))
                        {
                            // IdleState에 접근 가능
                            (*IdleState)->SetLinkAnimationSequence(FFbxManager::GetAnimSequenceByName(animNames[i]));
                        }

                       
                    }
                    
                }
            }
            ImGui::EndCombo();
        }

        static int SelectedAnim2Index = -1;
        static std::string SelectedAnim2Label = "NONE";

        if (SelectedAnim2Index >= 0 && SelectedAnim2Index < animNames.Num())
        {
            SelectedAnim2Label = animNames[SelectedAnim2Index].ToAnsiString();
        }

        if (ImGui::BeginCombo("Walk - Anim2", SelectedAnim2Label.c_str()))
        {
            for (int i = 0; i < animNames.Num(); ++i)
            {
                if (ImGui::Selectable(*animNames[i], false))
                {
                    SelectedAnim2Index = i;
                    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(SkeletalComp->GetAnimationInstance());
                    UPreviewAnimInstance* PreviewAnimInstance = Cast<UPreviewAnimInstance>(SkeletalComp->GetAnimationInstance());
                    if (MyAnimInstance)
                    {
                        MyAnimInstance->Anim2 = FFbxManager::GetAnimSequenceByName(animNames[i]);
                    }
                    else if (PreviewAnimInstance) {
                        PreviewAnimInstance->Anim2 = FFbxManager::GetAnimSequenceByName(animNames[i]);

                        uint32 StateKey = FName("Walk").GetComparisonIndex();

                        if (UAnimNode_State** WalkState = PreviewAnimInstance->GetAnimSM()->StateContainer.Find(StateKey))
                        {
                            // IdleState에 접근 가능
                            (*WalkState)->SetLinkAnimationSequence(FFbxManager::GetAnimSequenceByName(animNames[i]));
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }

        static int SelectedAnim3Index = -1;
        static std::string SelectedAnim3Label = "NONE";

        if (SelectedAnim3Index >= 0 && SelectedAnim3Index < animNames.Num())
        {
            SelectedAnim3Label = animNames[SelectedAnim3Index].ToAnsiString();
        }

        if (ImGui::BeginCombo("Jump - Anim3", SelectedAnim3Label.c_str()))
        {
            for (int i = 0; i < animNames.Num(); ++i)
            {
                if (ImGui::Selectable(*animNames[i], false))
                {
                    SelectedAnim3Index = i;
                    UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(SkeletalComp->GetAnimationInstance());
                    UPreviewAnimInstance* PreviewAnimInstance = Cast<UPreviewAnimInstance>(SkeletalComp->GetAnimationInstance());
                    if (PreviewAnimInstance) {
                        PreviewAnimInstance->Anim3 = FFbxManager::GetAnimSequenceByName(animNames[i]);

                        uint32 StateKey = FName("Jump").GetComparisonIndex();

                        if (UAnimNode_State** JumpState = PreviewAnimInstance->GetAnimSM()->StateContainer.Find(StateKey))
                        {
                            // IdleState에 접근 가능
                            (*JumpState)->SetLinkAnimationSequence(FFbxManager::GetAnimSequenceByName(animNames[i]));
                        }
                    }

                }
            }
            ImGui::EndCombo();
        }

        UPreviewAnimInstance* Instance = Cast<UPreviewAnimInstance>(SkeletalComp->GetAnimationInstance());
        if (Instance) {
            UAnimationStateMachine* StateMachine = Instance->GetAnimSM();
            if (StateMachine) {
                TArray<FAnimTransition>& Transitions = StateMachine->GetTransitions();
                
                for (int i = 0; i < Transitions.Num(); ++i)
                {

                    FAnimTransition& Transition = Transitions[i];

                    // From/To 이름 추출
                    FString FromName = (Transition.FromState) ? Transition.FromState->GetStateNameFName().ToString() : TEXT("None");
                    FString ToName = (Transition.ToState) ? Transition.ToState->GetStateNameFName().ToString() : TEXT("None");

                    FString LabelPrefix = FString::Printf(TEXT("%s -> %s"), *FromName, *ToName);
                    std::string LabelID = *LabelPrefix;


                    ImGui::Separator();
                    ImGui::Text("Transition: %s", LabelID.c_str());

                    // Blend Duration 슬라이더
                    float BlendValue = Transition.Duration;
                    std::string BlendLabel = "Blend Duration##" + LabelID;
                    if (ImGui::DragFloat(BlendLabel.c_str(), &BlendValue, 0.01f, 0.0f, 10.0f, "%.2f"))
                    {
                        Transition.Duration = BlendValue;
                    }

                    // 조건 체크박스 (예시: Idle → Walk만 있는 경우라면 단일 ID로 처리해도 됨)
                    std::string CondLabel = "Condition##" + LabelID;
                    bool bCondition = Instance->bTransition[i];
                    if (ImGui::Checkbox(CondLabel.c_str(), &bCondition))
                    {
                        Instance->bTransition[i] = bCondition;
                    }
                }

            }
        }

        if (ImGui::Button("Preview"))
        {
            UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
            if (Engine)
            {
                USkeletalMesh* SkeletalMesh = SkeletalComp->GetSkeletalMesh();
                if (SkeletalMesh)
                {
                    for (auto Actor : Engine->GetPreviewWorld(GEngineLoop.SkeletalMeshViewerAppWnd)->GetActiveLevel()->Actors)
                    {
                        if (Actor && Actor->IsA<AItemActor>())
                        {
                            USkeletalMeshComponent* PreviewSkeletalMeshComponent = Cast<AItemActor>(Actor)->GetComponentByClass<USkeletalMeshComponent>();
                            PreviewSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
                        }
                    }
                }
            }
            GEngineLoop.Show(GEngineLoop.SkeletalMeshViewerAppWnd);
        }

        if (ImGui::Button("Animation"))
        {
            UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
            if (Engine)
            {
                USkeletalMesh* SkeletalMesh = SkeletalComp->GetSkeletalMesh();
                if (SkeletalMesh)
                {
                    for (auto Actor : Engine->GetPreviewWorld(GEngineLoop.AnimationViewerAppWnd)->GetActiveLevel()->Actors)
                    {
                        if (Actor && Actor->IsA<AItemActor>())
                        {
                            USkeletalMeshComponent* PreviewSkeletalMeshComponent = Cast<AItemActor>(Actor)->GetComponentByClass<USkeletalMeshComponent>();
                            PreviewSkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
                        }
                    }
                }
            }
            GEngineLoop.Show(GEngineLoop.AnimationViewerAppWnd);
        }

        if (ImGui::Button("Apply"))
        {
            UEditorEngine* Engine = Cast<UEditorEngine>(GEngine);
            if (Engine)
            {
                //if (SkeletalMesh)
                {
                    for (auto Actor : Engine->GetPreviewWorld(GEngineLoop.SkeletalMeshViewerAppWnd)->GetActiveLevel()->Actors)
                    {
                        if (Actor && Actor->IsA<AItemActor>())
                        {
                            USkeletalMeshComponent* PreviewSkeletalMeshComponent = Cast<AItemActor>(Actor)->GetComponentByClass<USkeletalMeshComponent>();
                            SkeletalComp->CurrentPose = (PreviewSkeletalMeshComponent->CurrentPose);
                        }
                    }
                }
            }
            GEngineLoop.Show(GEngineLoop.SkeletalMeshViewerAppWnd);
        }

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForModifySkeletalBone(USkeletalMeshComponent* SkeletalMeshComponent)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("ModifyBone", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        ImGui::Text("Bone");

        const TMap<int, FString> boneIndexToName = SkeletalMeshComponent->GetBoneIndexToName();
        std::function<void(int)> CreateNode = [&CreateNode, &SkeletalMeshComponent, &boneIndexToName](int InParentIndex)
        {
            TArray<int> childrenIndices = SkeletalMeshComponent->GetChildrenOfBone(InParentIndex);

            ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_None;
            if (childrenIndices.Num() == 0)
                Flags |= ImGuiTreeNodeFlags_Leaf;
            if (SkeletalMeshComponent->SelectedBoneIndex == InParentIndex)
                Flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
            bool NodeOpen = ImGui::TreeNodeEx(GetData(boneIndexToName[InParentIndex]), Flags);
            
            if (ImGui::IsItemClicked())
                SkeletalMeshComponent->SelectedBoneIndex = InParentIndex;
            if (NodeOpen)
            {
                for (int childIndex : childrenIndices)
                {
                    CreateNode(childIndex);
                }
                ImGui::TreePop();
            }
        };

        TArray<int> rootIndices = SkeletalMeshComponent->GetChildrenOfBone(-1);
        for (int rootIndex : rootIndices)
        {
            CreateNode(rootIndex);
        }
        
        if (SkeletalMeshComponent->SelectedBoneIndex > -1)
        {
            ImGui::Text("Bone Pose");
            ImGui::SameLine();
            if (ImGui::Button("Reset Pose"))
            {
                SkeletalMeshComponent->ResetPose();
            }
            FTransform& boneTransform = SkeletalMeshComponent->CurrentPose[SkeletalMeshComponent->SelectedBoneIndex];
            FImGuiWidget::DrawVec3Control("Location", boneTransform.Translation, 0, 85);
            ImGui::Spacing();

            
            FRotator SkelRotator = boneTransform.Rotation.Rotator();
            FImGuiWidget::DrawRot3Control("Rotation", SkelRotator, 0, 85);
            boneTransform.Rotation = FQuat(SkelRotator);
            ImGui::Spacing();

            FImGuiWidget::DrawVec3Control("Scale", boneTransform.Scale3D, 0, 85);

            ImGui::Text("Reference Pose");
            FReferenceSkeleton skeleton;
            SkeletalMeshComponent->GetSkeletalMesh()->GetRefSkeleton(skeleton);
            FTransform refTransform = skeleton.RawRefBonePose[SkeletalMeshComponent->SelectedBoneIndex];
            FImGuiWidget::DrawVec3Control("refLocation", refTransform.Translation, 0, 85);
            ImGui::Spacing();

            FRotator refSkelRotator = refTransform.Rotation.Rotator();
            FImGuiWidget::DrawRot3Control("refRotation", refSkelRotator, 0, 85);
            refTransform.Rotation = FQuat(refSkelRotator);
            ImGui::Spacing();

            FImGuiWidget::DrawVec3Control("refScale", refTransform.Scale3D, 0, 85);
        }
        //FVector& SelectedLocation = SkeletalMeshComponent->GetSkeletalMesh()->RefSkeleton.RawRefBonePose[SkeletalMeshComponent->SelectedBoneIndex].Translation;

        ImGui::Spacing();

        ImGui::TreePop();
    }
    ImGui::PopStyleColor(); 
}

void PropertyEditorPanel::RenderForAmbientLightComponent(UAmbientLightComponent* AmbientLightComponent) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("AmbientLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawColorProperty("Light Color",
            [&]() { return AmbientLightComponent->GetLightColor(); },
            [&](FLinearColor c) { AmbientLightComponent->SetLightColor(c); });
        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForDirectionalLightComponent(UDirectionalLightComponent* DirectionalLightComponent) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("DirectionalLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawColorProperty("Light Color",
            [&]() { return DirectionalLightComponent->GetLightColor(); },
            [&](FLinearColor c) { DirectionalLightComponent->SetLightColor(c); });

        float Intensity = DirectionalLightComponent->GetIntensity();
        if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 150.0f, "%.1f"))
        {
            DirectionalLightComponent->SetIntensity(Intensity);
        }

        FVector LightDirection = DirectionalLightComponent->GetDirection();
        FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);

        // --- Cast Shadows 체크박스 추가 ---
        bool bCastShadows = DirectionalLightComponent->GetCastShadows(); // 현재 상태 가져오기
        if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
        {
            DirectionalLightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
            // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
            // 예: PointlightComponent->MarkRenderStateDirty();
        }
        ImGui::Text("ShadowMap");

        // 분할된 개수만큼 CSM 해당 SRV 출력
        const uint32& NumCascades = FEngineLoop::Renderer.ShadowManager->GetNumCasCades();
        for (uint32 i = 0; i < NumCascades; ++i)
        {
           //ImGui::Image(reinterpret_cast<ImTextureID>(FEngineLoop::Renderer.ShadowManager->GetDirectionalShadowCascadeDepthRHI()->ShadowSRVs[i]), ImVec2(200, 200));
            //ImGui::SameLine();
        }
        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForPointLightComponent(UPointLightComponent* PointlightComponent) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("PointLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawColorProperty("Light Color",
            [&]() { return PointlightComponent->GetLightColor(); },
            [&](FLinearColor c) { PointlightComponent->SetLightColor(c); });

        float Intensity = PointlightComponent->GetIntensity();
        if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
        {
            PointlightComponent->SetIntensity(Intensity);
        }

        float Radius = PointlightComponent->GetRadius();
        if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
        {
            PointlightComponent->SetRadius(Radius);
        }
        // --- Cast Shadows 체크박스 추가 ---
        bool bCastShadows = PointlightComponent->GetCastShadows(); // 현재 상태 가져오기
        if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
        {
            PointlightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
            // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
            // 예: PointlightComponent->MarkRenderStateDirty();
        }

        ImGui::Text("ShadowMap");

        FShadowCubeMapArrayRHI* pointRHI = FEngineLoop::Renderer.ShadowManager->GetPointShadowCubeMapRHI();
        const char* faceNames[] = { "+X", "-X", "+Y", "-Y", "+Z", "-Z" };
        float imageSize = 128.0f;
        int index = PointlightComponent->GetPointLightInfo().ShadowMapArrayIndex;
        // CubeMap이므로 6개의 ShadowMap을 그립니다.
        for (int i = 0; i < 6; ++i)
        {
            ID3D11ShaderResourceView* faceSRV = pointRHI->ShadowFaceSRVs[index][i];
            if (faceSRV)
            {
                ImGui::Image(reinterpret_cast<ImTextureID>(faceSRV), ImVec2(imageSize, imageSize));
                ImGui::SameLine();
                ImGui::Text("%s", faceNames[i]);
            }
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForSpotLightComponent(USpotLightComponent* SpotLightComponent) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("SpotLight Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawColorProperty("Light Color",
            [&]() { return SpotLightComponent->GetLightColor(); },
            [&](FLinearColor c) { SpotLightComponent->SetLightColor(c); });

        float Intensity = SpotLightComponent->GetIntensity();
        if (ImGui::SliderFloat("Intensity", &Intensity, 0.0f, 160.0f, "%.1f"))
        {
            SpotLightComponent->SetIntensity(Intensity);
        }

        float Radius = SpotLightComponent->GetRadius();
        if (ImGui::SliderFloat("Radius", &Radius, 0.01f, 200.f, "%.1f"))
        {
            SpotLightComponent->SetRadius(Radius);
        }

        FVector LightDirection = SpotLightComponent->GetDirection();
        FImGuiWidget::DrawVec3Control("Direction", LightDirection, 0, 85);

        float InnerConeAngle = SpotLightComponent->GetInnerDegree();
        float OuterConeAngle = SpotLightComponent->GetOuterDegree();
        if (ImGui::DragFloat("InnerConeAngle", &InnerConeAngle, 0.5f, 0.0f, 80.0f))
        {
            OuterConeAngle = std::max(InnerConeAngle, OuterConeAngle);

            SpotLightComponent->SetInnerDegree(InnerConeAngle);
            SpotLightComponent->SetOuterDegree(OuterConeAngle);
        }

        if (ImGui::DragFloat("OuterConeAngle", &OuterConeAngle, 0.5f, 0.0f, 80.0f))
        {
            InnerConeAngle = std::min(OuterConeAngle, InnerConeAngle);

            SpotLightComponent->SetOuterDegree(OuterConeAngle);
            SpotLightComponent->SetInnerDegree(InnerConeAngle);
        }

        // --- Cast Shadows 체크박스 추가 ---
        bool bCastShadows = SpotLightComponent->GetCastShadows(); // 현재 상태 가져오기
        if (ImGui::Checkbox("Cast Shadows", &bCastShadows)) // 체크박스 UI 생성 및 상호작용 처리
        {
            SpotLightComponent->SetCastShadows(bCastShadows); // 변경된 상태 설정
            // 필요하다면, 상태 변경에 따른 즉각적인 렌더링 업데이트 요청 로직 추가
            // 예: PointlightComponent->MarkRenderStateDirty();
        }

        ImGui::Text("ShadowMap");
        ImGui::Image(reinterpret_cast<ImTextureID>(FEngineLoop::Renderer.ShadowManager->GetSpotShadowDepthRHI()->ShadowSRVs[0]), ImVec2(200, 200));

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForLightCommon(ULightComponentBase* LightComponent) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    // --- "Override Camera" 버튼 추가 ---
    if (ImGui::Button("Override Camera with Light's Perspective"))
    {
        // 1. 라이트의 월드 위치 및 회전 가져오기
        FVector LightLocation = LightComponent->GetWorldLocation();

        FVector Forward = FVector(1.f, 0.f, 0.0f);
        Forward = JungleMath::FVectorRotate(Forward, LightLocation);
        FVector LightForward = Forward;
        FRotator LightRotation = LightComponent->GetWorldRotation();
        FVector LightRotationVector;
        LightRotationVector.X = LightRotation.Roll;
        LightRotationVector.Y = -LightRotation.Pitch;
        LightRotationVector.Z = LightRotation.Yaw;

        // 2. 활성 에디터 뷰포트 클라이언트 가져오기 (!!! 엔진별 구현 필요 !!!)
        std::shared_ptr<FEditorViewportClient> ViewportClient = GEngineLoop.GetLevelEditor()->GetActiveViewportClient(); // 위에 정의된 헬퍼 함수 사용 (또는 직접 구현)

        // 3. 뷰포트 클라이언트가 유효하면 카메라 설정
        if (ViewportClient)
        {
            ViewportClient->PerspectiveCamera.SetLocation(LightLocation + LightForward); // 카메라 위치 설정 함수 호출
            ViewportClient->PerspectiveCamera.SetRotation(LightRotationVector); // 카메라 회전 설정 함수 호출

            // 필요시 뷰포트 강제 업데이트/다시 그리기 호출
            // ViewportClient->Invalidate();
        }
        else
        {
            // 뷰포트 클라이언트를 찾을 수 없음 (오류 로그 등)
            // UE_LOG(LogTemp, Warning, TEXT("Active Viewport Client not found."));
        }
    }
    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForProjectileMovementComponent(UProjectileMovementComponent* ProjectileComp)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("Projectile Movement Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
#if USE_UPROPERTY_IMGUI == false
        float InitialSpeed = ProjectileComp->GetInitialSpeed();
        if (ImGui::InputFloat("InitialSpeed", &InitialSpeed, 0.f, 10000.0f, "%.1f"))
        {
            ProjectileComp->SetInitialSpeed(InitialSpeed);
        }

        float MaxSpeed = ProjectileComp->GetMaxSpeed();
        if (ImGui::InputFloat("MaxSpeed", &MaxSpeed, 0.f, 10000.0f, "%.1f"))
        {
            ProjectileComp->SetMaxSpeed(MaxSpeed);
        }

        float Gravity = ProjectileComp->GetGravity();
        if (ImGui::InputFloat("Gravity", &Gravity, 0.f, 10000.f, "%.1f"))
        {
            ProjectileComp->SetGravity(Gravity);
        }

        float ProjectileLifetime = ProjectileComp->GetLifetime();
        if (ImGui::InputFloat("Lifetime", &ProjectileLifetime, 0.f, 10000.f, "%.1f"))
        {
            ProjectileComp->SetLifetime(ProjectileLifetime);
        }

        FVector CurrentVelocity = ProjectileComp->GetVelocity();

        float Velocity[3] = { CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z };

        if (ImGui::InputFloat3("Velocity", Velocity, "%.1f"))
        {
            ProjectileComp->SetVelocity(FVector(Velocity[0], Velocity[1], Velocity[2]));
        }
#else
        RenderProperties(ProjectileComp);
#endif
        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForTextComponent(UTextComponent* TextComponent) const
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Text Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        if (TextComponent)
        {
            TextComponent->SetTexture(L"Assets/Texture/font.png");
            TextComponent->SetRowColumnCount(106, 106);
            FWString wText = TextComponent->GetText();
            int Len = WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, nullptr, 0, nullptr, nullptr);
            std::string u8Text(Len, '\0');
            WideCharToMultiByte(CP_UTF8, 0, wText.c_str(), -1, u8Text.data(), Len, nullptr, nullptr);

            static char Buf[256];
            strcpy_s(Buf, u8Text.c_str());

            ImGui::Text("Text: ", Buf);
            ImGui::SameLine();
            ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
            if (ImGui::InputText("##Text", Buf, 256, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                TextComponent->ClearText();
                int wLen = MultiByteToWideChar(CP_UTF8, 0, Buf, -1, nullptr, 0);
                FWString wNewText(wLen, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, Buf, -1, wNewText.data(), wLen);
                TextComponent->SetText(wNewText.c_str());
            }
            ImGui::PopItemFlag();
        }
        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForExponentialHeightFogComponent(UHeightFogComponent* FogComponent)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::TreeNodeEx("Exponential Height Fog", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
#if USE_UPROPERTY_IMGUI == false
        FLinearColor CurrColor = FogComponent->GetFogColor();

        float R = CurrColor.R;
        float G = CurrColor.G;
        float B = CurrColor.B;
        float A = CurrColor.A;
        float H, S, V;
        float LightColor[4] = { R, G, B, A };

        // Fog Color
        if (ImGui::ColorPicker4("##Fog Color", LightColor,
            ImGuiColorEditFlags_DisplayRGB |
            ImGuiColorEditFlags_NoSidePreview |
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_Float))
        {
            R = LightColor[0];
            G = LightColor[1];
            B = LightColor[2];
            A = LightColor[3];
            FogComponent->SetFogColor(FLinearColor(R, G, B, A));
        }
        RGBToHSV(R, G, B, H, S, V);
        // RGB/HSV
        bool ChangedRGB = false;
        bool ChangedHSV = false;

        // RGB
        ImGui::PushItemWidth(50.0f);
        if (ImGui::DragFloat("R##R", &R, 0.001f, 0.f, 1.f)) ChangedRGB = true;
        ImGui::SameLine();
        if (ImGui::DragFloat("G##G", &G, 0.001f, 0.f, 1.f)) ChangedRGB = true;
        ImGui::SameLine();
        if (ImGui::DragFloat("B##B", &B, 0.001f, 0.f, 1.f)) ChangedRGB = true;
        ImGui::Spacing();

        // HSV
        if (ImGui::DragFloat("H##H", &H, 0.1f, 0.f, 360)) ChangedHSV = true;
        ImGui::SameLine();
        if (ImGui::DragFloat("S##S", &S, 0.001f, 0.f, 1)) ChangedHSV = true;
        ImGui::SameLine();
        if (ImGui::DragFloat("V##V", &V, 0.001f, 0.f, 1)) ChangedHSV = true;
        ImGui::PopItemWidth();
        ImGui::Spacing();

        if (ChangedRGB && !ChangedHSV)
        {
            // RGB -> HSV
            RGBToHSV(R, G, B, H, S, V);
            FogComponent->SetFogColor(FLinearColor(R, G, B, A));
        }
        else if (ChangedHSV && !ChangedRGB)
        {
            // HSV -> RGB
            HSVToRGB(H, S, V, R, G, B);
            FogComponent->SetFogColor(FLinearColor(R, G, B, A));
        }

        float FogDensity = FogComponent->GetFogDensity();
        if (ImGui::SliderFloat("Density", &FogDensity, 0.00f, 3.0f))
        {
            FogComponent->SetFogDensity(FogDensity);
        }

        float FogDistanceWeight = FogComponent->GetFogDistanceWeight();
        if (ImGui::SliderFloat("Distance Weight", &FogDistanceWeight, 0.00f, 3.0f))
        {
            FogComponent->SetFogDistanceWeight(FogDistanceWeight);
        }

        float FogHeightFallOff = FogComponent->GetFogHeightFalloff();
        if (ImGui::SliderFloat("Height Fall Off", &FogHeightFallOff, 0.001f, 0.15f))
        {
            FogComponent->SetFogHeightFalloff(FogHeightFallOff);
        }

        float FogStartDistance = FogComponent->GetStartDistance();
        if (ImGui::SliderFloat("Start Distance", &FogStartDistance, 0.00f, 50.0f))
        {
            FogComponent->SetStartDistance(FogStartDistance);
        }

        float FogEndtDistance = FogComponent->GetEndDistance();
        if (ImGui::SliderFloat("End Distance", &FogEndtDistance, 0.00f, 50.0f))
        {
            FogComponent->SetEndDistance(FogEndtDistance);
        }
#else
        RenderProperties(FogComponent);
#endif

        ImGui::TreePop();
    }
    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForShapeComponent(UShapeComponent* ShapeComponent)
{
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (USphereComponent* SphereComp = Cast<USphereComponent>(ShapeComponent))
    {
        if (ImGui::TreeNodeEx("Sphere Collision", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
#if USE_UPROPERTY_IMGUI == false
            float Radius = SphereComp->GetRadius();
            ImGui::Text("Radius");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Radius", &Radius, 0.01f, 0.f, 1000.f))
            {
                SphereComp->SetRadius(Radius);
            }
#else
            RenderProperties(SphereComp);
#endif
            ImGui::TreePop();
        }
    }

    if (UBoxComponent* BoxComp = Cast<UBoxComponent>(ShapeComponent))
    {
        if (ImGui::TreeNodeEx("Box Collision", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
#if USE_UPROPERTY_IMGUI == false
            FVector Extent = BoxComp->GetBoxExtent();

            float Extents[3] = { Extent.X, Extent.Y, Extent.Z };

            ImGui::Text("Extent");
            ImGui::SameLine();
            if (ImGui::DragFloat3("##Extent", Extents, 0.01f, 0.f, 1000.f))
            {
                BoxComp->SetBoxExtent(FVector(Extents[0], Extents[1], Extents[2]));
            }
#else
            RenderProperties(BoxComp);
#endif

            ImGui::TreePop();
        }
    }

    if (UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(ShapeComponent))
    {
        if (ImGui::TreeNodeEx("Capsule Collision", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
        {
#if USE_UPROPERTY_IMGUI == false
            float HalfHeight = CapsuleComp->GetHalfHeight();
            float Radius = CapsuleComp->GetRadius();

            ImGui::Text("HalfHeight");
            ImGui::SameLine();
            if (ImGui::DragFloat("##HalfHeight", &HalfHeight, 0.01f, 0.f, 1000.f)) {
                CapsuleComp->SetHalfHeight(HalfHeight);
            }

            ImGui::Text("Radius");
            ImGui::SameLine();
            if (ImGui::DragFloat("##Radius", &Radius, 0.01f, 0.f, 1000.f)) {
                CapsuleComp->SetRadius(Radius);
            }
#else
            RenderProperties(CapsuleComp);
#endif
            ImGui::TreePop();
        }
    }

    ImGui::PopStyleColor();
}

void PropertyEditorPanel::RenderForSpringArmComponent(USpringArmComponent* SpringArmComponent)
{
    if (ImGui::TreeNodeEx("SpringArm", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
    {
#if USE_UPROPERTY_IMGUI == false
        // --- TargetOffset (FVector) ---
        float TargetOffsetValues[3] = {
            SpringArmComponent->TargetOffset.X,
            SpringArmComponent->TargetOffset.Y,
            SpringArmComponent->TargetOffset.Z
        };
        if (ImGui::DragFloat3("TargetOffset", TargetOffsetValues, 1.0f, -1000.0f, 1000.0f))
        {
            SpringArmComponent->TargetOffset.X = TargetOffsetValues[0];
            SpringArmComponent->TargetOffset.Y = TargetOffsetValues[1];
            SpringArmComponent->TargetOffset.Z = TargetOffsetValues[2];
        }
        ImGui::Spacing();

        // --- Floats: ArmLength / ProbeSize ---
        ImGui::DragFloat("ArmLength", &SpringArmComponent->TargetArmLength, 1.0f, 0.0f, 1000.0f);
        ImGui::DragFloat("ProbeSize", &SpringArmComponent->ProbeSize, 0.1f, 0.0f, 30.0f);
        ImGui::Spacing();

        // --- Bools (2 per line) ---
        bool DoCollisionTest = SpringArmComponent->bDoCollisionTest;
        if (ImGui::Checkbox("DoCollisionTest", &DoCollisionTest))
            SpringArmComponent->bDoCollisionTest = DoCollisionTest;
        ImGui::SameLine();
        bool UsePawnControlRotation = SpringArmComponent->bUsePawnControlRotation;
        if (ImGui::Checkbox("UsePawnControlRotation", &UsePawnControlRotation))
            SpringArmComponent->bUsePawnControlRotation = UsePawnControlRotation;

        bool UseAbsolRot = SpringArmComponent->IsUsingAbsoluteRotation();
        if (ImGui::Checkbox("UseAbsoluteRot", &UseAbsolRot))
            SpringArmComponent->SetUsingAbsoluteRotation(UseAbsolRot);

        bool InheritPitch = SpringArmComponent->bInheritPitch;
        if (ImGui::Checkbox("InheritPitch", &InheritPitch))
            SpringArmComponent->bInheritPitch = InheritPitch;
        ImGui::SameLine();
        bool InheritYaw = SpringArmComponent->bInheritYaw;
        if (ImGui::Checkbox("InheritYaw", &InheritYaw))
            SpringArmComponent->bInheritYaw = InheritYaw;

        bool InheritRoll = SpringArmComponent->bInheritRoll;
        if (ImGui::Checkbox("InheritRoll", &InheritRoll))
            SpringArmComponent->bInheritRoll = InheritRoll;
        ImGui::SameLine();
        bool EnableCameraLag = SpringArmComponent->bEnableCameraLag;
        if (ImGui::Checkbox("EnableCameraLag", &EnableCameraLag))
            SpringArmComponent->bEnableCameraLag = EnableCameraLag;

        bool EnableCameraRotationLag = SpringArmComponent->bEnableCameraRotationLag;
        if (ImGui::Checkbox("EnableCameraRotationLag", &EnableCameraRotationLag))
            SpringArmComponent->bEnableCameraRotationLag = EnableCameraRotationLag;
        ImGui::SameLine();
        bool UseCameraLagSubstepping = SpringArmComponent->bUseCameraLagSubstepping;
        if (ImGui::Checkbox("UseCameraLagSubstepping", &UseCameraLagSubstepping))
            SpringArmComponent->bUseCameraLagSubstepping = UseCameraLagSubstepping;
        ImGui::Spacing();

        // --- Lag speeds / limits ---
        ImGui::DragFloat("LocSpeed", &SpringArmComponent->CameraLagSpeed, 0.1f, 0.0f, 100.0f);

        ImGui::DragFloat("RotSpeed", &SpringArmComponent->CameraRotationLagSpeed, 0.1f, 0.0f, 100.0f);
        //ImGui::NewLine();
        ImGui::DragFloat("LagMxStep", &SpringArmComponent->CameraLagMaxTimeStep, 0.005f, 0.0f, 1.0f);
        ImGui::SameLine();
        ImGui::DragFloat("LogMDist", &SpringArmComponent->CameraLagMaxDistance, 1.0f, 0.0f, 1000.0f);
#else
        RenderProperties(SpringArmComponent);
#endif
        ImGui::TreePop();
    }
}

void PropertyEditorPanel::RenderForMaterial(UStaticMeshComponent* StaticMeshComp)
{
    if (StaticMeshComp->GetStaticMesh() == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    if (ImGui::TreeNodeEx("Materials", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        for (uint32 i = 0; i < StaticMeshComp->GetNumMaterials(); ++i)
        {
            if (ImGui::Selectable(GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    std::cout << GetData(StaticMeshComp->GetMaterialSlotNames()[i].ToString()) << std::endl;
                    SelectedMaterialIndex = i;
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }

        if (ImGui::Button("    +    ")) {
            IsCreateMaterial = true;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("SubMeshes", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen)) // 트리 노드 생성
    {
        const auto Subsets = StaticMeshComp->GetStaticMesh()->GetRenderData().MaterialSubsets;
        for (uint32 i = 0; i < Subsets.Num(); ++i)
        {
            std::string temp = "subset " + std::to_string(i);
            if (ImGui::Selectable(temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    StaticMeshComp->SetselectedSubMeshIndex(i);
                    SelectedStaticMeshComp = StaticMeshComp;
                }
            }
        }
        const std::string Temp = "clear subset";
        if (ImGui::Selectable(Temp.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (ImGui::IsMouseDoubleClicked(0))
                StaticMeshComp->SetselectedSubMeshIndex(-1);
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();

    if (SelectedMaterialIndex != -1)
    {
        RenderMaterialView(SelectedStaticMeshComp->GetMaterial(SelectedMaterialIndex));
    }
    if (IsCreateMaterial) {
        RenderCreateMaterialView();
    }
}

void PropertyEditorPanel::RenderMaterialView(UMaterial* Material)
{
    ImGui::SetNextWindowSize(ImVec2(380, 400), ImGuiCond_Once);
    ImGui::Begin("Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    const FVector MatDiffuseColor = Material->GetMaterialInfo().DiffuseColor;
    const FVector MatSpecularColor = Material->GetMaterialInfo().SpecularColor;
    const FVector MatAmbientColor = Material->GetMaterialInfo().AmbientColor;
    const FVector MatEmissiveColor = Material->GetMaterialInfo().EmissiveColor;

    const float DiffuseR = MatDiffuseColor.X;
    const float DiffuseG = MatDiffuseColor.Y;
    const float DiffuseB = MatDiffuseColor.Z;
    constexpr float DiffuseA = 1.0f;
    float DiffuseColorPick[4] = { DiffuseR, DiffuseG, DiffuseB, DiffuseA };

    ImGui::Text("Material Name |");
    ImGui::SameLine();
    ImGui::Text(*Material->GetMaterialInfo().MaterialName);
    ImGui::Separator();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", reinterpret_cast<float*>(&DiffuseColorPick), BaseFlag))
    {
        const FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        Material->SetDiffuse(NewColor);
    }

    const float SpecularR = MatSpecularColor.X;
    const float SpecularG = MatSpecularColor.Y;
    const float SpecularB = MatSpecularColor.Z;
    constexpr float SpecularA = 1.0f;
    float SpecularColorPick[4] = { SpecularR, SpecularG, SpecularB, SpecularA };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", reinterpret_cast<float*>(&SpecularColorPick), BaseFlag))
    {
        const FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        Material->SetSpecular(NewColor);
    }

    const float AmbientR = MatAmbientColor.X;
    const float AmbientG = MatAmbientColor.Y;
    const float AmbientB = MatAmbientColor.Z;
    constexpr float AmbientA = 1.0f;
    float AmbientColorPick[4] = { AmbientR, AmbientG, AmbientB, AmbientA };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", reinterpret_cast<float*>(&AmbientColorPick), BaseFlag))
    {
        const FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        Material->SetAmbient(NewColor);
    }

    const float EmissiveR = MatEmissiveColor.X;
    const float EmissiveG = MatEmissiveColor.Y;
    const float EmissiveB = MatEmissiveColor.Z;
    constexpr float EmissiveA = 1.0f;
    float EmissiveColorPick[4] = { EmissiveR, EmissiveG, EmissiveB, EmissiveA };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", reinterpret_cast<float*>(&EmissiveColorPick), BaseFlag))
    {
        const FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        Material->SetEmissive(NewColor);
    } 
    
    TArray<FTextureInfo>& Textures = Material->GetMaterialInfo().TextureInfos;
    ImGui::Text("Texture Diffuse : %s", Textures[0].TexturePath.c_str());
    ImGui::Text("Texture Specular : %s", Textures[1].TexturePath.c_str());
    ImGui::Text("Texture Normal : %s", Textures[2].TexturePath.c_str());
    ImGui::Text("Texture Emissive : %s", Textures[3].TexturePath.c_str());
    ImGui::Text("Texture Alpha : %s", Textures[4].TexturePath.c_str());
    ImGui::Text("Texture Ambient : %s", Textures[5].TexturePath.c_str());
    ImGui::Text("Texture Shininess : %s", Textures[6].TexturePath.c_str());
    ImGui::Text("Texture Metallic : %s", Textures[7].TexturePath.c_str());
    ImGui::Text("Texture Roughness : %s", Textures[8].TexturePath.c_str());


    for (const auto& Texture : Textures)
    {
        ImGui::Text("Texture : %s", Texture.TexturePath.c_str());
    }


    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Choose Material");
    ImGui::Spacing();

    ImGui::Text("Material Slot Name |");
    ImGui::SameLine();
    ImGui::Text(GetData(SelectedStaticMeshComp->GetMaterialSlotNames()[SelectedMaterialIndex].ToString()));

    ImGui::Text("Override Material |");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160);
    // 메테리얼 이름 목록을 const char* 배열로 변환
    std::vector<const char*> MaterialChars;
    for (const auto& Material : FObjManager::GetMaterials()) {
        MaterialChars.push_back(*Material.Value->GetMaterialInfo().MaterialName);
    }

    //// 드롭다운 표시 (currentMaterialIndex가 범위를 벗어나지 않도록 확인)
    //if (currentMaterialIndex >= FManagerGetMaterialNum())
    //    currentMaterialIndex = 0;

    if (ImGui::Combo("##MaterialDropdown", &CurMaterialIndex, MaterialChars.data(), FObjManager::GetMaterialNum())) {
        UMaterial* Material = FObjManager::GetMaterial(MaterialChars[CurMaterialIndex]);
        SelectedStaticMeshComp->SetMaterial(SelectedMaterialIndex, Material);
    }

    if (ImGui::Button("Close"))
    {
        SelectedMaterialIndex = -1;
        SelectedStaticMeshComp = nullptr;
    }

    ImGui::End();
}

void PropertyEditorPanel::RenderCreateMaterialView()
{
    ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
    ImGui::Begin("Create Material Viewer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav);

    static ImGuiSelectableFlags BaseFlag = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoAlpha;

    ImGui::Text("New Name");
    ImGui::SameLine();
    static char MaterialName[256] = "New Material";
    // 기본 텍스트 입력 필드
    ImGui::SetNextItemWidth(128);
    if (ImGui::InputText("##NewName", MaterialName, IM_ARRAYSIZE(MaterialName))) {
        tempMaterialInfo.MaterialName = MaterialName;
    }

    const FVector MatDiffuseColor = tempMaterialInfo.DiffuseColor;
    const FVector MatSpecularColor = tempMaterialInfo.SpecularColor;
    const FVector MatAmbientColor = tempMaterialInfo.AmbientColor;
    const FVector MatEmissiveColor = tempMaterialInfo.EmissiveColor;
    const float MatEmissiveIntensity = tempMaterialInfo.EmissiveIntensity;

    const float DiffuseR = MatDiffuseColor.X;
    const float DiffuseG = MatDiffuseColor.Y;
    const float DiffuseB = MatDiffuseColor.Z;
    constexpr float DiffuseA = 1.0f;
    float DiffuseColorPick[4] = { DiffuseR, DiffuseG, DiffuseB, DiffuseA };

    ImGui::Text("Set Property");
    ImGui::Indent();

    ImGui::Text("  Diffuse Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##Color", reinterpret_cast<float*>(&DiffuseColorPick), BaseFlag))
    {
        const FVector NewColor = { DiffuseColorPick[0], DiffuseColorPick[1], DiffuseColorPick[2] };
        tempMaterialInfo.DiffuseColor = NewColor;
    }

    const float SpecularR = MatSpecularColor.X;
    const float SpecularG = MatSpecularColor.Y;
    const float SpecularB = MatSpecularColor.Z;
    constexpr float SpecularA = 1.0f;
    float SpecularColorPick[4] = { SpecularR, SpecularG, SpecularB, SpecularA };

    ImGui::Text("Specular Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Specular##Color", reinterpret_cast<float*>(&SpecularColorPick), BaseFlag))
    {
        const FVector NewColor = { SpecularColorPick[0], SpecularColorPick[1], SpecularColorPick[2] };
        tempMaterialInfo.SpecularColor = NewColor;
    }

    const float AmbientR = MatAmbientColor.X;
    const float AmbientG = MatAmbientColor.Y;
    const float AmbientB = MatAmbientColor.Z;
    constexpr float AmbientA = 1.0f;
    float AmbientColorPick[4] = { AmbientR, AmbientG, AmbientB, AmbientA };

    ImGui::Text("Ambient Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Ambient##Color", reinterpret_cast<float*>(&AmbientColorPick), BaseFlag))
    {
        const FVector NewColor = { AmbientColorPick[0], AmbientColorPick[1], AmbientColorPick[2] };
        tempMaterialInfo.AmbientColor = NewColor;
    }

    const float EmissiveR = MatEmissiveColor.X;
    const float EmissiveG = MatEmissiveColor.Y;
    const float EmissiveB = MatEmissiveColor.Z;
    constexpr float EmissiveA = 1.0f;
    float EmissiveColorPick[4] = { EmissiveR, EmissiveG, EmissiveB, EmissiveA };

    ImGui::Text("Emissive Color");
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Emissive##Color", reinterpret_cast<float*>(&EmissiveColorPick), BaseFlag))
    {
        const FVector NewColor = { EmissiveColorPick[0], EmissiveColorPick[1], EmissiveColorPick[2] };
        tempMaterialInfo.EmissiveColor = NewColor;
        tempMaterialInfo.EmissiveIntensity = 1.f;
    }
    ImGui::Unindent();

    ImGui::NewLine();
    if (ImGui::Button("Create Material")) {
        FObjManager::CreateMaterial(tempMaterialInfo);
    }

    ImGui::NewLine();
    if (ImGui::Button("Close"))
    {
        IsCreateMaterial = false;
    }

    ImGui::End();
}

void PropertyEditorPanel::OnResize(HWND hWnd)
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
