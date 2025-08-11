#pragma once
#include "Engine.h"
#include "Actors/Player.h"

/*
    Editor 모드에서 사용될 엔진.
    UEngine을 상속받아 Editor에서 사용 될 기능 구현.
    내부적으로 PIE, Editor World 두 가지 형태로 관리.
*/

class AActor;
class USceneComponent;

class UEditorEngine : public UEngine
{
    DECLARE_CLASS(UEditorEngine, UEngine)

public:
    UEditorEngine() = default;

    virtual void Init() override;
    virtual void Tick(float DeltaTime) override;

    UWorld* PIEWorld = nullptr;
    UWorld* EditorWorld = nullptr;

    void StartPIE();
    void EndPIE();

    // 주석은 UE에서 사용하던 매개변수.
    FWorldContext& GetEditorWorldContext(/*bool bEnsureIsGWorld = false*/);
    FWorldContext* GetPIEWorldContext(/*int32 WorldPIEInstance = 0*/);

public:
    void SelectActor(AActor* InActor);
    void DeselectActor(AActor* InActor);
    bool CanSelectActor(AActor* InActor) const;
    AActor* GetSelectedActor() const;

    void HoverActor(AActor* InActor);

    
    void NewLevel();

    void SelectComponent(USceneComponent* InComponent);
    void DeselectComponent(USceneComponent* InComponent);
    bool CanSelectComponent(USceneComponent* InComponent) const;
    USceneComponent* GetSelectedComponent() const;

    void HoverComponent(USceneComponent* InComponent);

public:
    AEditorPlayer* GetEditorPlayer();
    
private:
    AEditorPlayer* EditorPlayer = nullptr;

};



