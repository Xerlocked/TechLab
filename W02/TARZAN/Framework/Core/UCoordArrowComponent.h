#pragma once
#include "UPrimitiveComponent.h"
class UCoordArrowComponent :
    public UPrimitiveComponent
{
public:
    UCoordArrowComponent();
    ~UCoordArrowComponent();
    void Render();
    void Update();

    void Set(FVector origin, FVector direction);
    FVector GetOrigin();
    FVector GetDirection();
private:
    FVector OriginWhenSpawned = { 0,0,0 };
    FVector DirectionWhenSpawned = { 0,0,0 };
};

