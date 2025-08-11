#pragma once
#include "./Framework/Core/USceneComponent.h"

class UCameraComponent :public USceneComponent {
public:
	virtual void Update();
	virtual void Render();
	void RenderUI();
	FMatrix View();
	FMatrix Projection();
	FMatrix OrthgonalProjection();
	FMatrix PerspectiveProjection();
	bool orthogonal = false;
	float fieldOfView = 60.f;
	float orthoScale = 0.5f;
	float aspectRatio = SCR_WIDTH / (float)SCR_HEIGHT;
	float nearDistance = 1.01f, farDistance = 100.f;
	float mouseSensitive = 0.5f;
	void UpdateRatio(float aratio) { aspectRatio = aratio; }

	bool allowKeyboardInput = true;
private:
	//FMatrix CalculateLookAtLHS(FVector front, FVector up, FVector pos);
};

