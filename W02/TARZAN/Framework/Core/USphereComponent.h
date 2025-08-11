#pragma once
#include "Framework/Core/UPrimitiveComponent.h"

class USphereComponent :
    public UPrimitiveComponent
{
public:
    USphereComponent();
    virtual void Update();

    //debug
    virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist);
};

