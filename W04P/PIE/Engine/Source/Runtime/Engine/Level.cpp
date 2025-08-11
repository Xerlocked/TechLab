#include "Level.h"
#include "GameFramework/Actor.h"
#include "Engine/FLoaderOBJ.h"
#include "Components/SkySphereComponent.h"
#include "UObject/Object.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
    Release();
}

void ULevel::Initialize()
{
    //SpawnObject(OBJ_CUBE);
    FManagerOBJ::CreateStaticMesh("Assets/Dodge/Dodge.obj");

    /*FManagerOBJ::CreateStaticMesh("Assets/SkySphere.obj");
    AActor* SpawnedActor = SpawnActor<AActor>();
    USkySphereComponent* skySphere = SpawnedActor->AddComponent<USkySphereComponent>();
    skySphere->SetStaticMesh(FManagerOBJ::GetStaticMesh(L"SkySphere.obj"));
    skySphere->GetStaticMesh()->GetMaterials()[0]->Material->SetDiffuse(FVector((float)32 / 255, (float)171 / 255, (float)191 / 255));*/
}

void ULevel::Tick(float DeltaTime)
{
    for (AActor* Actor : Actors)
    {
        if (Actor)
            Actor->Tick(DeltaTime);
    }

    // SpawnActor()에 의해 Actor가 생성된 경우, 여기서 BeginPlay 호출
    for (AActor* Actor : PendingBeginPlayActors)
    {
        Actor->BeginPlay();
    }
    PendingBeginPlayActors.Empty();
}

void ULevel::Release()
{
    for (AActor* Actor : Actors)
    {
        Actor->EndPlay(EEndPlayReason::WorldTransition);

        TSet<UActorComponent*> Components = Actor->GetComponents();
        for (UActorComponent* Component : Components)
        {
            GUObjectArray.MarkRemoveObject(Component);
        }
        GUObjectArray.MarkRemoveObject(Actor);
    }
    Actors.Empty();

    GUObjectArray.ProcessPendingDestroyObjects();
}

void ULevel::AddActor(AActor* Actor)
{
    if (Actor)
        Actors.Add(Actor);
}

void ULevel::RemoveActor(AActor* Actor)
{
    if (Actor->GetWorld() == nullptr)
        return;

    Actor->Destroyed();

    if (Actor->GetOwner())
    {
        Actor->SetOwner(nullptr);
    }

    TSet<UActorComponent*> Components = Actor->GetComponents();
    for (UActorComponent* Component : Components)
    {
        Component->DestroyComponent();
    }

    Actors.Remove(Actor);
    GUObjectArray.MarkRemoveObject(Actor);
}

TArray<AActor*> ULevel::GetActors()
{
    return Actors;
}

