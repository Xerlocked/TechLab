#pragma once
#include "PrimitiveComponent.h"

#include "Types/Types.h"
#include "Math/Matrix.h"

class URenderer;

class USphereComponent : public UPrimitiveComponent
{
public:
	USphereComponent();
	USphereComponent(URenderer* InRenderer);
	USphereComponent(URenderer* InRenderer, const FVector& InLocation);
	USphereComponent(const USphereComponent&);
	~USphereComponent();

	static UClass* GetClass();

	UClass* GetInstanceClass() const override;

	void Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix);

	bool CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult) override;

private:
	void Initialize();
};