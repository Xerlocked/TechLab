#include "CameraComponent.h"
#include "Input/InputManager.h"
#include "DirectXMath.h"
#include "Math/Matrix.h"

UCameraComponent::UCameraComponent()
{
	Initialize();
}

UCameraComponent::UCameraComponent(const UCameraComponent&)
{
}

UCameraComponent::~UCameraComponent()
{
}

void UCameraComponent::Initialize()
{
	FieldOfView = 60.f;

	bIsOrthogonal = false;

	NearZ = 0.1f;
	FarZ = 1000.0f;

	CameraPosition.X = 0.0f;
	CameraPosition.Y = 0.0f;
	CameraPosition.Z = -5.0f;

	CameraRotation.X = 0.0;
	CameraRotation.Y = 0.0;
	CameraRotation.Z = 0.0;

	// 목표점을 월드 좌표의 고정된 위치로 설정
	//CameraTarget = FVector(0.0f, 0.0f, 0.0f);
}

FMatrix UCameraComponent::CreateLookAt()
{
	// 1. 카메라 회전 행렬 생성 (입력 각도는 라디안 단위)
	RotationMatrix = FMatrix::CreateRotationRollPitchYaw(CameraRotation.X, CameraRotation.Y, CameraRotation.Z);

	// 2. 회전 행렬을 사용해 기본 forward와 up 벡터 변환
	//    (LH 좌표계에서 기본 forward = (0, 0, 1), up = (0, 1, 0))
	FVector Forward = RotationMatrix * FVector(0.0f, 0.0f, 1.0f);
	FVector Up = RotationMatrix * FVector(0.0f, 1.0f, 0.0f);

	// 3. LookAt 좌표 계산: 카메라 위치에서 Forward 방향으로 한 단위 이동
	// -> 목표점은 CameraTarget을 사용하여 고정된 좌표로 지정
	FVector LookAt = CameraPosition + Forward;

	// 4. 정규직교 좌표계 구성
	//    ForwardNormalized는 카메라가 바라보는 실제 방향
	FVector ForwardNormalized = (LookAt - CameraPosition).Normalized();
	// LH 좌표계: Right = normalize(cross(Up, ForwardNormalized))
	FVector Right = Up.Cross(ForwardNormalized).Normalized();
	// 보정된 Up 벡터: NewUp = cross(ForwardNormalized, Right)
	FVector NewUp = ForwardNormalized.Cross(Right);

	// 5. 평행 이동 요소 계산: 각 축 벡터와 카메라 위치의 내적의 음수
	float D0 = -Right.Dot(CameraPosition);
	float D1 = -NewUp.Dot(CameraPosition);
	float D2 = -ForwardNormalized.Dot(CameraPosition);

	// 6. 뷰 행렬 구성 (row-major 방식)
	/*float m[4][4] = {
		{ Right.X,         Right.Y,         Right.Z,         D0 },
		{ NewUp.X,         NewUp.Y,         NewUp.Z,         D1 },
		{ ForwardNormalized.X, ForwardNormalized.Y, ForwardNormalized.Z, D2 },
		{ 0.0f,            0.0f,            0.0f,            1.0f }
	};*/

	float m[4][4] = {
		{ Right.X,		NewUp.X,    ForwardNormalized.X,	0.0f },
		{ Right.Y,		NewUp.Y,    ForwardNormalized.Y,	0.0f },
		{ Right.Z,		NewUp.Z,    ForwardNormalized.Z,	0.0f },
		{	   D0,           D1,				     D2,	1.0f }
	};


	return FMatrix(m);
}

UClass* UCameraComponent::GetClass()
{
	static UClass CameraClass("UCameraComponent", UObject::GetClass());
	return &CameraClass;
}

UClass* UCameraComponent::GetInstanceClass() const
{
	return GetClass();
}

void UCameraComponent::Render()
{
	// 마우스 이동량을 이용한 회전 처리
	UpdateRotationFromMouse();

	float MoveSpeed = 0.3f;

	// 키보드 입력 처리
	if (FInputManager::GetInst().GetKey('W') == EKeyState::Held)
	{
		CameraPosition += RotationMatrix * FVector(0.0f, 0.0f, MoveSpeed);
	}
	if (FInputManager::GetInst().GetKey('A') == EKeyState::Held)
	{
		CameraPosition -= RotationMatrix * FVector(MoveSpeed, 0.0f, 0.0f);
	}
	if (FInputManager::GetInst().GetKey('S') == EKeyState::Held)
	{
		CameraPosition -= RotationMatrix * FVector(0.0f, 0.0f, MoveSpeed);
	}
	if (FInputManager::GetInst().GetKey('D') == EKeyState::Held)
	{
		CameraPosition += RotationMatrix * FVector(MoveSpeed, 0.0f, 0.0f);
	}

	ViewMatrix = CreateLookAt();

	// 마우스 델타 초기화
	FInputManager::GetInst().ResetMouseDeltas();
}

void UCameraComponent::SetPosition(float x, float y, float z)
{
	CameraPosition.X = x;
	CameraPosition.Y = y;
	CameraPosition.Z = z;
}

void UCameraComponent::SetRotation(float x, float y, float z)
{
	CameraRotation.X = x;
	CameraRotation.Y = y;
	CameraRotation.Z = z;
}

void UCameraComponent::SetViewportSize(float Width, float Height)
{
	// 뷰포트 설정
	ViewportWidth = Width;
	ViewportHeight = Height;
}

FVector UCameraComponent::GetPosition()
{
	return CameraPosition;
}

FVector UCameraComponent::GetRotation()
{
	return CameraRotation;
}


void UCameraComponent::GetViewMatrix(FMatrix& InViewMatrix)
{
	InViewMatrix = ViewMatrix;
	return;
}

void UCameraComponent::UpdateRotationFromMouse()
{
	// 마우스 우클릭이 눌리지 않았다면 회전하지 않음 
	if (FInputManager::GetInst().GetKey(VK_RBUTTON) != EKeyState::Held)
	{
		return;
	}

	// 마우스 이동량 가져오기
	float DeltaX = FInputManager::GetInst().GetMouseDeltaX();
	float DeltaY = FInputManager::GetInst().GetMouseDeltaY();


	// 감도 설정
	float Sensitivity = 0.002f;

	// 회전 값 업데이트
	CameraRotation.Y += DeltaX * Sensitivity; // Yaw (좌우 회전)
	CameraRotation.X += DeltaY * Sensitivity; // Pitch (상하 회전)

	// Pitch 회전 제한 (고개가 너무 많이 꺾이는 것 방지)
	float MaxPitch = DirectX::XMConvertToRadians(89.0f);
	float MinPitch = DirectX::XMConvertToRadians(-89.0f);
	CameraRotation.X = (CameraRotation.X < MinPitch) ? MinPitch : (CameraRotation.X > MaxPitch) ? MaxPitch : CameraRotation.X;
}

FVector UCameraComponent::GetRayDirection(int ScreenX, int ScreenY)
{
	float SinFov;
	float CosFov;
	float ToRadian = DirectX::XMConvertToRadians(0.5 * FieldOfView);
	DirectX::XMScalarSinCos(&SinFov, &CosFov, ToRadian);
	float ProjectionHeight = CosFov / SinFov;
	float ProjectionWidth = ProjectionHeight / (ViewportWidth / ViewportHeight);
	//스크린 좌표를 정규화된 장치 좌표로 변환
	float NDC_X = ((2.0f * ScreenX) / ViewportWidth - 1.0f) / ProjectionWidth;
	float NDC_Y = (1.0f - (2.0f * ScreenY) / ViewportHeight) / ProjectionHeight;

	FVector RayOrigin = FVector(0.0f, 0.0f, 0.0f);
	FVector RayDirection = FVector(NDC_X, NDC_Y, 1.0f);

	// 뷰 좌표를 월드 좌표로 변환
	FMatrix InverseViewMatrix = ViewMatrix.Inverse();
	RayOrigin = InverseViewMatrix * RayOrigin;
	RayDirection = FMatrix::TransformDirection(InverseViewMatrix, RayDirection);
	RayDirection = RayDirection.Normalized();

	return RayDirection;
}