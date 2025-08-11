#pragma once
#include "TextComponent.h"

class UTextUUID : public UTextComponent
{
    DECLARE_CLASS(UTextUUID, UTextComponent)

public:
    UTextUUID();

    virtual int CheckRayIntersection(
        FVector& rayOrigin,
        FVector& rayDirection, float& pfNearHitDistance
    ) const override;
    void SetUUID(uint32 UUID);
};
