#pragma once

#include "Components/PrimitiveComponent.h"

class ULineComponent : public UPrimitiveComponent
{
public:
	ULineComponent();
	ULineComponent(URenderer* InRenderer, FVector InLocation);
	ULineComponent(URenderer* InRenderer);
	ULineComponent(const ULineComponent&);
	~ULineComponent();

	static UClass* GetClass();
	UClass* GetInstanceClass() const override;

	bool CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult) override;

	void Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix);
private:
	void Initialize();
};

