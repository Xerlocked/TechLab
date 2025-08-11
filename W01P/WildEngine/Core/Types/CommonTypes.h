#pragma once
#include "Math/Vector.h"
#include "Components/SceneComponent.h"

struct FVertexType
{
    float x, y, z;    // Position
    float r, g, b, a; // Color
};

enum class EPrimitiveType
{
    Cube,
    Sphere,
    Triangle
};

enum class EGizmoType
{
    Translation,
    Rotation,
    Scale
};

struct FHitResult
{
    bool bHit;         // 충돌 여부
    float Distance;    // 충돌한 위치까지의 거리
    FVector HitLocation; // 충돌 위치
    USceneComponent* HitObject; // 충돌한 오브젝트

    FHitResult() : bHit(false), Distance(FLT_MAX), HitLocation(), HitObject(nullptr) {}
};