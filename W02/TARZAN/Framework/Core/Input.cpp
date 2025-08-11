#include "stdafx.h"
#include "Input.h"
#include "Framework/Core/UCoordArrowComponent.h"
#include "UWorld.h"

//class UWorld;
extern UWorld* gMainScene;

Input* Input::_instance = nullptr;

bool Input::Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	//m_mouseX = 0;
	//m_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	assert(SUCCEEDED(result));

	// 키보드 초기화
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	assert(SUCCEEDED(result));
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	//result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND); // 창모드 (https://m.blog.naver.com/PostView.naver?blogId=fish19&logNo=120183923039)
	assert(SUCCEEDED(result));
	result = m_keyboard->Acquire();
	assert(SUCCEEDED(result));

	// 마우스 초기화
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	assert(SUCCEEDED(result));
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//result = m_mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	assert(SUCCEEDED(result));
	result = m_mouse->Acquire();
	assert(SUCCEEDED(result));

	m_hWnd = hwnd;
	return true;
}

void Input::Shutdown() {
	// Release the mouse.
	if ( m_mouse ) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}
	// Release the keyboard.
	if ( m_keyboard ) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}
	// Release the main interface to direct input.
	if ( m_directInput ) {
		m_directInput->Release();
		m_directInput = 0;
	}
	return;
}

void Input::ResizeScreen(int sWidth, int sHeight)
{
	m_screenHeight = sHeight;
	m_screenWidth = sWidth;
}

bool Input::Frame() {
	bool result;

	result = ReadKeyboard();
	if ( !result )
		return false;

	result = ReadMouse();
	if ( !result )
		return false;

	ProcessInput();

	return true;
}

bool Input::IsKeyDown(char keycode) {
	if ( m_keyboardState[keycode] & 0x80 )
		return true;
	return false;
}

bool Input::IsKeyPressed(char keycode) {
	if ( m_keyboardState[keycode] & 0x80 && !(m_keyboardStatePrevious[keycode] & 0x80) )
		return true;
	return false;
}

bool Input::IsKeyReleased(char keycode) {
	if ( !(m_keyboardState[keycode] & 0x80) && m_keyboardStatePrevious[keycode] & 0x80 )
		return true;
	return false;
}

bool Input::IsMouseButtonDown(char button) {
	if ( m_mouseState.rgbButtons[button] & 0x80 )
		return true;
	return false;
}

bool Input::IsMouseButtonPressed(char button) {
	if ( m_mouseState.rgbButtons[button] & 0x80 && !(m_mouseStatePrevious.rgbButtons[button] & 0x80) )
		return true;
	return false;
}

bool Input::IsMouseButtonReleased(char button) {
	if ( !(m_mouseState.rgbButtons[button] & 0x80) && m_mouseStatePrevious.rgbButtons[button] & 0x80 )
		return true;
	return false;
}

void Input::GetMouseLocation(int& mouse_x, int& mouse_y) {
	//mouseX = m_mouseX;
	//mouseY = m_mouseY;
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_hWnd, &p);
	mouse_x = p.x;
	mouse_y = p.y;
	return;
}

void Input::GetMouseDelta(int& mouse_x, int& mouse_y) {
	mouse_x = m_mouseState.lX;
	mouse_y = m_mouseState.lY;
}

void Input::GetMouseWheel(int& mouse_w) {
	mouse_w = m_mouseState.lZ;
}

bool Input::ReadKeyboard() {
	HRESULT result = 0;

	memcpy(m_keyboardStatePrevious, m_keyboardState, sizeof(m_keyboardState));
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (!ImGui::GetIO().WantTextInput)
		result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	else
		m_keyboard->Unacquire();
	if ( FAILED(result) ) {
		if ( (result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED) ) {
			m_keyboard->Acquire();
		} else {
			return false;
		}
	}

	return true;
}

bool Input::ReadMouse() {
	HRESULT result;

	memcpy(&m_mouseStatePrevious, &m_mouseState, sizeof(DIMOUSESTATE));
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if ( FAILED(result) ) {
		// If the mouse lost focus or was not acquired then try to get control back.
		if ( (result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED) ) {
			m_mouse->Acquire();
		} else {
			return false;
		}
	}
	return true;
}

void Input::ProcessInput() {

	//m_mouseX += m_mouseState.lX;
	//m_mouseY += m_mouseState.lY;

	//if ( m_mouseX < 0 ) { m_mouseX = 0; }
	//if ( m_mouseY < 0 ) { m_mouseY = 0; }

	//if ( m_mouseX > m_screenWidth ) { m_mouseX = m_screenWidth; }
	//if ( m_mouseY > m_screenHeight ) { m_mouseY = m_screenHeight; }

	return;
}
void Input::GetMouseRay(FVector& rayOrigin, FVector& rayDirection,
	const FMatrix& viewMatrix, const FMatrix& projectionMatrix) {
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(m_hWnd, &cursorPos);

	float ndcX = ((float)cursorPos.x / m_screenWidth) * 2.0f - 1.0f;
	float ndcY = 1.0f - ((float)cursorPos.y / m_screenHeight) * 2.0f;

	FVector4 nearPoint = FVector4(ndcX, ndcY, 0.f, 1.f);
	FVector4 farPoint = FVector4(ndcX, ndcY, 1.f, 1.f);
	//ImGui::Begin("GetMouseRay");
	//ImGui::Text("nearPoint : %f %f %f \nfarPoint : %f %f %f", nearPoint.x, nearPoint.y, nearPoint.z, farPoint.x, farPoint.y, farPoint.z);
	//ImGui::End();
	FMatrix invProj = projectionMatrix.Inverse();

	nearPoint = nearPoint * invProj;
	farPoint = farPoint * invProj;

	FMatrix invView = viewMatrix.Inverse();
	nearPoint = nearPoint * invView;
	farPoint = farPoint * invView;

	rayOrigin = nearPoint.xyz()/nearPoint.w;
	
	rayDirection = (farPoint / farPoint.w - nearPoint/ nearPoint.w).xyz();

}

UCoordArrowComponent* Input::SpawnMouseRay(const FMatrix& viewMatrix, const FMatrix& projectionMatrix, const FMatrix& debugMatrix)
{
	FVector o, d;
	FMatrix viewProjInv = (viewMatrix * projectionMatrix).Inverse();
	GetMouseRay(o, d, viewMatrix, projectionMatrix);

	auto a = gMainScene->SpawnCoordArrowActor();
	auto s = d.Magnitude();
	//auto x = -asin(d.y);
	auto x = atan2(d.y, d.z);
	//auto x = -asin(d.y);

	auto y = -atan2(d.x, d.z);
	auto z = atan2(124, 3212);

	z = atan2(d.z, d.x);
	x = atan2(d.y, sqrt(d.x * d.x + d.z * d.z));


	a->SetRelativeRotation({ x, y, 0 });
	a->SetRelativeLocation(o);
	a->SetRelativeScale3D({ s ,s,s});

	a->Set(o, d);
	
	//ImGui::Begin("SpawnMouseRay");
	//ImGui::Text("spawn ray's info (loc rot scale)%f, %f, %f, %f, %f, %f, %f", a->GetRelativeLocation().x, a->GetRelativeLocation().y, a->GetRelativeLocation().z, a->GetRelativeRotation().x, a->GetRelativeRotation().y, 0, a->GetRelativeScale3D().x);
	//ImGui::Text("spawn ray's info (front right up)%f, %f, %f, %f, %f, %f, %f", a->Front().x, a->Front().y, a->Front().z, a->Right().x, a->Right().y, 0, a->Right().x);
	//ImGui::End();
	
	return a;





	//FVector o, d;
	//FMatrix viewProjInv = (viewMatrix * projectionMatrix).Inverse();

	//// world coordinate에서의 ray
	//GetMouseRay(o, d, viewMatrix, projectionMatrix);

	//// d가 Front이고 o가 Position인 View Matrix 만들고 분해
	//// 가 아니라 그냥 
	//auto translateInv = FMatrix({
	//	1,0,0,0,
	//	0,1,0,0,
	//	0,0,1,0,
	//	-d.x, -d.y, -d.z, 1
	//	});

	//float denom = 1 / d.Magnitude();
	//auto scaleInv = FMatrix({
	//	denom,0,0,0,
	//	0,denom,0,0,
	//	0,0,denom,0,
	//	0,0,0,1
	//	});

	////auto rotate = scaleInv *
	//auto a = new UCoordArrowComponent;

	//auto x = -asin(d.y);
	//auto y = atan2(d.x, d.z);
	//a->SetRelativeRotation({ x, y, 0 });
	//
	////a->SetRelativeLocation(o);
	////a->SetRelativeScale3D(s);


	//auto s = d.Magnitude();
	////a->SetRelativeLocation(o);
	//a->SetRelativeScale3D(s);

	//return;
	////auto x = -asin(d.y);
	////auto y = atan2(d.x, d.z);
	////a->SetRelativeRotation({x, y, 0});


	//	//a->SetValuesFromMatrix(viewProjInv);
	//
	////a->SetRelativeLocation(o);
	//// 오일러 변환으로 변경(x,y,z)

	///*auto pitch = std::atan2(d.y, sqrt(d.x * d.x + d.z * d.z));
	//auto yaw = std::atan2(d.z,d.x);

	//a->SetRelativeRotationX(pitch);
	//a->SetRelativeRotationY(yaw);*/


	//if(0 && debugMatrix != FMatrix::Identity)
	//{
	//	a->ISDEBUG = true;
	//	a->DEBUG_TRANSFORMATION_OVERRIDE = debugMatrix;
	//}
	
}
