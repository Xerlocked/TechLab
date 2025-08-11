#pragma once
#include "SceneComponent.h"
#include "Primitive.h"
#include "Renderer/URenderer.h"

class UPrimitiveComponent : public USceneComponent
{
public:
	UPrimitiveComponent();
	UPrimitiveComponent(const UPrimitiveComponent&);
	~UPrimitiveComponent();

	void Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix);

	// 위치, 회전, 스케일 값을 설정할 수 있는 setter 함수들
	void SetRelativeLocation(const FVector& NewLocation) { RelativeLocation = NewLocation; }
	void SetRelativeRotation(const FVector& NewRotation) { RelativeRotation = NewRotation; }
	void SetRelativeScale3D(const FVector& NewScale3D) { RelativeScale3D = NewScale3D; }

	static UClass* GetClass();

	UClass* GetInstanceClass() const override;

	virtual bool CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult) = 0;

	URenderer* Renderer;

	UINT NumVertices;
	ID3D11Buffer* VertexBuffer;

	float rot;
};