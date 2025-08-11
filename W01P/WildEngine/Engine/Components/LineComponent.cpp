#include "LineComponent.h"

ULineComponent::ULineComponent()
{
}

ULineComponent::ULineComponent(URenderer* InRenderer, FVector InLocation)
{
	Renderer = InRenderer;
	RelativeLocation = InLocation;
	Initialize();
}

ULineComponent::ULineComponent(URenderer* InRenderer)
{
	Renderer = InRenderer;
	Initialize();
}

ULineComponent::ULineComponent(const ULineComponent&)
{
}

ULineComponent::~ULineComponent()
{
}

UClass* ULineComponent::GetClass()
{
	static UClass CubeClass("UCubeComponent", UObject::GetClass());
	return &CubeClass;
}

UClass* ULineComponent::GetInstanceClass() const
{
	return GetClass();
}

bool ULineComponent::CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult)
{
	return false;
}

void ULineComponent::Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix)
{
	FMatrix Translation = FMatrix::Translation(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
	FMatrix Rotation = FMatrix::CreateRotationRollPitchYaw(RelativeRotation.X, RelativeRotation.Y, RelativeRotation.Z);
	FMatrix Scaling = FMatrix::Scaling(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z);

	WorldTransform = Scaling * Rotation * Translation; // 스케일링 * 회전 * 이동

	// 셰이더 상수 버퍼 업데이트
	Renderer->UpdateShaderParameters(WorldTransform, ViewMatrix, ProjectionMatrix);

	Renderer->RenderPrimitive(VertexBuffer, NumVertices);
}

void ULineComponent::Initialize()
{
	NumVertices = sizeof(grid_vertices) / sizeof(FVertexType);
	VertexBuffer = Renderer->CreateVertexBuffer(grid_vertices, sizeof(grid_vertices));
}
