#include "World/World.h"
#include "Actors/Player.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Level.h"

UWorld::UWorld()
    : Camera(nullptr)
    , EditorPlayer(nullptr)
    , WorldType(EWorldType::Editor)
{
}

UWorld::~UWorld()
{
}

void UWorld::Initialize()
{
    CreateBaseObject();
        
    Level = new ULevel();
    Level->Initialize();
}

void UWorld::Tick(float DeltaTime)
{
    if (LocalGizmo)
        LocalGizmo->Tick(DeltaTime);
    if (EditorPlayer)
        EditorPlayer->Tick(DeltaTime);

    if (Camera)
        Camera->TickComponent(DeltaTime);
    
    for (AActor* Actor : Level->GetActors())
    {
        if (Actor && Actor->IsActorTickEnabled())
        {
            Actor->Tick(DeltaTime);
        }
    }
}

void UWorld::Release()
{
    Level->Release();
    delete Level;
    ReleaseBaseObject();
}

UWorld* UWorld::DuplicateWorldForPIE(UWorld* EditorWorld)
{
    return nullptr;
}

void UWorld::InitializeActorsForPlay()
{
    for (AActor* Actor : Level->GetActors())
    {
        if (Actor)
            Actor->BeginPlay();
    }
}

bool UWorld::IsPIEWorld() const
{
    return WorldType == EWorldType::PIE ? true : false;
}

void UWorld::CleanupWorld()
{
    
}

void UWorld::CreateBaseObject()
{
    if (!EditorPlayer)
        EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>();

    if (!Camera)
    {
        Camera = FObjectFactory::ConstructObject<UCameraComponent>();
        Camera->SetLocation(FVector(8.0f, 8.0f, 8.f));
        Camera->SetRotation(FVector(0.0f, 45.0f, -135.0f));
    }

    if (LocalGizmo == nullptr)
        LocalGizmo = FObjectFactory::ConstructObject<UTransformGizmo>();
}

void UWorld::ReleaseBaseObject()
{
    if (LocalGizmo)
    {
        delete LocalGizmo;
        LocalGizmo = nullptr;
    }

    if (Camera)
    {
        delete Camera;
        Camera = nullptr;
    }

    if (EditorPlayer)
    {
        delete EditorPlayer;
        EditorPlayer = nullptr;
    }

    if (PickingGizmo)
    {
        PickingGizmo = nullptr;
    }
}


void UWorld::SetPickingGizmo(UObject* Object)
{
	PickingGizmo = Cast<USceneComponent>(Object);
}