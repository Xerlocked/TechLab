#pragma once
#include "UPrimitiveComponent.h"
#include "UArrowComponent.h"

class UDiscComponent :
    public UPrimitiveComponent
{
public:
    UDiscComponent(EPrimitiveColor color);
    UDiscComponent();
    ~UDiscComponent();
    //virtual void Update();
    virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist);
};

