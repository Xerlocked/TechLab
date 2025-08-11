#pragma once
#include "Interface/ISingleton.h"
#include "Types/Types.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include <unordered_map>
#include <Windows.h>

enum class EKeyState
{
	None,		// 입력 없음
	Pressed,	// 처음 눌린 순간 (1프레임 동안만 유지)
	Held,		// 누르고 있는 상태 (Pressed 이후)
	Released	// 키에서 손을 뗀 순간 (1프레임 동안만 유지)
};

class FInputManager : public ISingleton<FInputManager>
{
public:
	void Init();
	void Tick(float DeltaTime);

	bool GetKeyState(int Key, EKeyState KeyState) const;
	EKeyState GetKey(int Key) const;

	// 마우스 입력 관련 함수 추가
	void ProcessMouseMovement(int CurrentX, int CurrentY);
	void ResetMouseDeltas();
	float GetMouseDeltaX() const
	{
		return MouseDeltaX;
	}
	float GetMouseDeltaY() const
	{
		return MouseDeltaY;
	}
	int GetMouseX() const
	{
		return LastMouseX;
	}
	int GetMouseY() const
	{
		return LastMouseY;
	}

private:
	friend class ISingleton<FInputManager>;
	FInputManager() = default;

	TMap<int, EKeyState> KeyStateMap;

	int LastMouseX = 0;
	int LastMouseY = 0;
	float MouseDeltaX = 0.0f;
	float MouseDeltaY = 0.0f;
};

