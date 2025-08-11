#pragma once
#include "PrimitiveComponent.h"

#include "Types/Types.h"
#include "Math/Matrix.h"

class URenderer;

class UCubeComponent : public UPrimitiveComponent
{
public:
	UCubeComponent();
	UCubeComponent(URenderer* InRenderer, FVector InLocation);
	UCubeComponent(URenderer* InRenderer);
	UCubeComponent(const UCubeComponent&);
	~UCubeComponent();

	static UClass* GetClass();
	UClass* GetInstanceClass() const override;

	void Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix);
	bool CheckRayIntersection(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult) override;

private:
	void Initialize();
};