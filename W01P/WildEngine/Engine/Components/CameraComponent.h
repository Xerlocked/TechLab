#pragma once

#include "Components/SceneComponent.h"
#include "Input/InputManager.h"

class UCameraComponent : public USceneComponent
{
public:
	UCameraComponent();
	UCameraComponent(const UCameraComponent&);
	~UCameraComponent();

	static UClass* GetClass();
	UClass* GetInstanceClass() const override;

	void Render();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	// 뷰포트 크기 설정
	void SetViewportSize(float Width, float Height);

	FVector GetPosition();
	FVector GetRotation();

	void GetViewMatrix(FMatrix& InViewMatrix);

	// 마우스로 회전 처리
	void UpdateRotationFromMouse();

	// Ray Casting 함수 추가
	FVector GetRayDirection(int ScreenX, int ScreenY);

public:
	// Default FOV : 90
	float FieldOfView;

	// Default Near : 0.1
	float NearZ;

	// Default Far : 1000
	float FarZ;

	// Perspective or Orthogonal
	bool bIsOrthogonal;

	// Camera Position
	FVector CameraPosition;

	// Camera Rotation
	FVector CameraRotation;
private:
	void Initialize();

	FMatrix CreateLookAt();


private:
	// Camera Target
	//FVector CameraTarget;

	// Camera ViewMatrix
	FMatrix ViewMatrix;
	FMatrix RotationMatrix;

	// Viewport 크기
	float ViewportWidth;
	float ViewportHeight;
};

