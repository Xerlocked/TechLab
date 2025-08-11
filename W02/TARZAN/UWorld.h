#pragma once
#include "stdafx.h"
#include "Framework/Core/UObject.h"
#include "Framework/Core/USphereComponent.h"
//#include "Framework/Core/UActorComponent.h"
#include "Framework/Core/UCubeComponent.h"
#include "Framework/Core/UPlaneComponent.h"
#include "Framework/Core/UArrowComponent.h"
#include "Framework/Core/UCoordArrowComponent.h"
#include "Framework/Core/UDiscComponent.h"
#include "Framework/Core/UDiscHollowComponent.h"


class UActorComponent;
class UCubeComponent;
class USphereComponent;
class UPlaneComponent;
class UCoordArrowComponent;

struct PrimitiveData {
    FVector Location;
    FVector Rotation;
    FVector Scale;
    FString Type;
};

class UWorld :
    public UObject
{
public:
    UWorld();
    ~UWorld();

    void Update();
    void Render();

    void AddActor(UActorComponent* comp);
    void RemoveActor(UActorComponent* comp);
    void ClearWorld();

    int GetActorCount() const;
    const TLinkedList<UActorComponent*>& GetActors() const;

    void ConvertNDC_VIEW(int mouse_X, int mouse_Y, FVector& pickPosition, FMatrix& viewMatrix);
    UActorComponent* PickingByRay(int mouse_X, int mouse_Y, float& distance);
    void SetAxisPicked(UArrowComponent* axisX, UArrowComponent* axisY, UArrowComponent* axisZ, EPrimitiveColor pickedAxis);

    UCameraComponent* SpawnCamera();
    UCubeComponent* SpawnCubeActor();
    USphereComponent* SpawnSphereActor();
    UPlaneComponent* SpawnPlaneActor();
    UCoordArrowComponent* SpawnCoordArrowActor();
    
    void SaveWorld(const FString& fileName);
    void LoadWorld(const FString& fileName);
    UDiscComponent* SpawnDiscActor();
    UDiscHollowComponent* SpawnDiscHollowActor();

private:
    TLinkedList<UActorComponent*> actorList = {};

    template <typename T>
    T* SpawnActor(bool addActorList = true);
    template <typename T>
    T* SpawnActor(FVector position, FVector rotation = FVector::Zero, FVector scal = FVector::One);
};

template<typename T>
inline T* UWorld::SpawnActor(bool addActorList)
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must be derived from UActorComponent");

    T* newActor = new T();

    if (!newActor)
        return nullptr;

    if (addActorList)
        AddActor(newActor);

    return newActor;
}

template<typename T>
inline T* UWorld::SpawnActor(FVector position, FVector rotation, FVector scal)
{
    static_assert(std::is_base_of<UActorComponent, T>::value, "T must be derived from UActorComponent");

    return nullptr;
}

