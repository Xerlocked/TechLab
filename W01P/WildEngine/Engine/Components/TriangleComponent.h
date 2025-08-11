#pragma once
#include "PrimitiveComponent.h"

#include "Types/Types.h"
#include "Math/Matrix.h"

class URenderer;

class UTriangleComponent : public UPrimitiveComponent
{
public:
	UTriangleComponent();
	UTriangleComponent(URenderer* InRenderer);
	UTriangleComponent(URenderer* InRenderer, const FVector& InLocation);
	UTriangleComponent(const UTriangleComponent&);
	~UTriangleComponent();

	static UClass* GetClass();
	UClass* GetInstanceClass() const override;

	void Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix);

	bool CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult) override;

private:
	void Initialize();
};

