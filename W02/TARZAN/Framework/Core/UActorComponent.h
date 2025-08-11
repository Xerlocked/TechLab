#pragma once
#include "Framework/Core/UObject.h"

class UActorComponent :public UObject {	
public:
	UActorComponent() {}
	virtual ~UActorComponent() {}

	virtual void Update() {};
	virtual void Render() {};
	//virtual void GenerateRayForPicking(FVector& pickPosition, FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection) {};
	virtual void GenerateRayForPicking(const FVector& pickPosition, const FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection) {}
	virtual int PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance) { return false; }
};

