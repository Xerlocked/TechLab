#pragma once
#include "UObject/ObjectMacros.h"

class AActor;
class UObject;
class UGizmoArrowComponent;
class UCameraComponent;
class AEditorPlayer;
class USceneComponent;
class UTransformGizmo;

enum EWorldType
{
    Editor,
    EditorPreview,
    PIE,
    Game,
};

class UWorld : public UObject
{
    DECLARE_CLASS(UWorld, UObject)

public:
    UWorld();
    virtual ~UWorld();

    virtual void Initialize();
    virtual void Tick(float DeltaTime);
    virtual void Release();

    static UWorld* DuplicateWorldForPIE(UWorld* EditorWorld);
    void InitializeActorsForPlay();
    bool IsPIEWorld() const;
    void CleanupWorld();

protected:
    ULevel* Level;
    EWorldType WorldType;

    UCameraComponent* Camera;
    AEditorPlayer* EditorPlayer;
    UTransformGizmo* LocalGizmo = nullptr;

private:
    USceneComponent* PickingGizmo = nullptr;

    void CreateBaseObject();
    void ReleaseBaseObject();

public:
    ULevel* GetLevel() const { return Level; }
    UCameraComponent* GetCamera() const { return Camera; }
    AEditorPlayer* GetEditorPlayer() const { return EditorPlayer; }
    UTransformGizmo* GetLocalGizmo() const { return LocalGizmo; }

    USceneComponent* GetPickingGizmo() const { return PickingGizmo; }
    void SetPickingGizmo(UObject* Object);
};