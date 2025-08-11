#include "InputManager.h"

void FInputManager::Init()
{
	// Unoredered_map에 키를 등록해줍니다. 
	// 예시) KeyStateMap[VK_SPACE] = EKeyState::None;
	// or KeyStateMap.insert(std::make_pair(VK_SPACE, EKeyState::None));
	// or KeyStateMap.emplace(VK_SPACE, EKeyState::None);
	KeyStateMap['W'] = EKeyState::None; KeyStateMap['A'] = EKeyState::None; KeyStateMap['S'] = EKeyState::None; KeyStateMap['D'] = EKeyState::None;
	KeyStateMap[VK_RBUTTON] = EKeyState::None;
	KeyStateMap[VK_LBUTTON] = EKeyState::None;
	KeyStateMap[0x31] = EKeyState::None; KeyStateMap[0x32] = EKeyState::None; KeyStateMap[0x33] = EKeyState::None;
}

void FInputManager::Tick(float DeltaTime)
{
	TMap<int, EKeyState>::iterator iter = KeyStateMap.begin();

	for (; KeyStateMap.end() != iter; ++iter)
	{
		bool bIsKeyPressed = (GetAsyncKeyState(iter->first) & 0x8000) != 0;

		switch (iter->second)
		{
		case EKeyState::None:
			if (bIsKeyPressed)
			{
				iter->second = EKeyState::Pressed; // 처음 눌린 순간
			}
			break;

		case EKeyState::Pressed:
			iter->second = bIsKeyPressed ? EKeyState::Held : EKeyState::Released; // 누르고 있는 상태 / 손을 뗀 순간
			break;

		case EKeyState::Held:
			if (!bIsKeyPressed)
			{
				iter->second = EKeyState::Released; // 손을 뗀 순간
			}
			break;

		case EKeyState::Released:
			iter->second = bIsKeyPressed ? EKeyState::Pressed : EKeyState::None; // 처음 눌린 순간 / 입력 없음
			break;
		}
	}
}

bool FInputManager::GetKeyState(int Key, EKeyState KeyState) const
{
	TMap<int, EKeyState>::const_iterator iter = KeyStateMap.find(Key);
	return (KeyStateMap.end() != iter && KeyState == iter->second);
}

EKeyState FInputManager::GetKey(int Key) const
{
	TMap<int, EKeyState >::const_iterator iter = KeyStateMap.find(Key);
	return (KeyStateMap.end() != iter) ? iter->second : EKeyState::None;
}

void FInputManager::ProcessMouseMovement(int CurrentX, int CurrentY)
{
	MouseDeltaX = static_cast<float>(CurrentX - LastMouseX);
	MouseDeltaY = static_cast<float>(CurrentY - LastMouseY);

	LastMouseX = CurrentX;
	LastMouseY = CurrentY;
}

void FInputManager::ResetMouseDeltas()
{
	MouseDeltaX = 0.0f;
	MouseDeltaY = 0.0f;
}
