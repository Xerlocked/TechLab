#include "LightComponent.h"
#include "UBillboardComponent.h"
#include "Math/JungleMath.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/ObjectFactory.h"

ULightComponentComponent::ULightComponentComponent()
{
    // FString name = "SpotLight";
    // SetName(name);
    InitializeLight();
}

ULightComponentComponent::~ULightComponentComponent()
{
    delete texture2D;
}
void ULightComponentComponent::SetColor(FVector4 newColor)
{
    color = newColor;
}

FVector4 ULightComponentComponent::GetColor() const
{
    return color;
}

float ULightComponentComponent::GetRadius() const
{
    return radius;
}

void ULightComponentComponent::SetRadius(float r)
{
    radius = r;
}

void ULightComponentComponent::InitializeLight()
{
    texture2D = FObjectFactory::ConstructObject<UBillboardComponent>();
    texture2D->SetTexture(L"Editor/Icon/SpotLight_64x.png");
    texture2D->InitializeComponent();
    texture2D->SetupAttachment(this);
    AABB.max = { 1.f,1.f,0.1f };
    AABB.min = { -1.f,-1.f,-0.1f };
    color = { 1,1,1,1 };
    radius = 5;
}

void ULightComponentComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

int ULightComponentComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    bool res =AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance);
    return res;
}

