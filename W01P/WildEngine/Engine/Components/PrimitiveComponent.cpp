#include "PrimitiveComponent.h"

UPrimitiveComponent::UPrimitiveComponent()
{
	RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	RelativeRotation = FVector(0.0f, 0.0f, 0.0f);
	RelativeScale3D = FVector(1.0f, 1.0f, 1.0f);
}

UPrimitiveComponent::UPrimitiveComponent(const UPrimitiveComponent&)
{
}

UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix)
{
    FMatrix Translation = FMatrix::Translation(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
    FMatrix Rotation = FMatrix::CreateRotationRollPitchYaw(RelativeRotation.X, RelativeRotation.Y, RelativeRotation.Z);
    FMatrix Scaling = FMatrix::Scaling(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z);

    WorldTransform = Scaling * Rotation * Translation; // 스케일링 * 회전 * 이동
    
    // 셰이더 상수 버퍼 업데이트
    Renderer->UpdateShaderParameters(WorldTransform, ViewMatrix, ProjectionMatrix);

    Renderer->RenderPrimitive(VertexBuffer, NumVertices);
}

UClass* UPrimitiveComponent::GetClass()
{
	static UClass PrimitiveClass("UPrimitiveComponent", UObject::GetClass());
	return &PrimitiveClass;
}

UClass* UPrimitiveComponent::GetInstanceClass() const
{
	return GetClass();
}

