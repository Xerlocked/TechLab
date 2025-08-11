#pragma once
#include "./Framework/Core/UPrimitiveComponent.h"
class UCubeComponent: public UPrimitiveComponent {
public:
	UCubeComponent();
	virtual void Update();

	//bool Intersects(FVector rayOrigin, FVector rayDirection);
	virtual bool IntersectsRay(const FVector& rayOrigin, const FVector& rayDir, float& dist);
};