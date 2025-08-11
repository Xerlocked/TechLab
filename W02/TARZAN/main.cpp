#include "stdafx.h"
#include "Framework/Core/CRenderer.h"
#include "Math\FVector.h"
#include "Math\FMatrix.h"
#include "Framework/Core/UCubeComponent.h"
#include "UWorld.h"
#include "Framework/Core/UPlaneComponent.h"
#include "Framework/Core/UCoordArrowComponent.h"
#include "Framework/Core/UArrowComponent.h"
#include "Framework/Core/UGizmoComponent.h"
#include "Framework/Core/UDiscHollowComponent.h"
extern int SCR_WIDTH;
extern int SCR_HEIGHT;
const int TARGET_FPS = 60;
const double TARGET_FRAMERATE = 1000.0 / TARGET_FPS;

UWorld* gMainScene;
UGizmoComponent* gGizmo;
GuiController* gGuiController;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if ( ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam) )
		return true;
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		if(gMainScene) {
			auto cam = CRenderer::Instance()->GetMainCamera();
			Input::Instance()->SpawnMouseRay(cam->View(), cam->PerspectiveProjection());
		}
		break;
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT tme = {};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd; 
		tme.dwHoverTime = 0;  
		TrackMouseEvent(&tme);
		break;
	}
	case WM_MOUSELEAVE:
		if (gMainScene) {

		}
		break;
	case WM_SIZE:
	{
		if (CRenderer::Instance()->GetGraphics() && CRenderer::Instance()->GetGraphics()->GetDevice() && CRenderer::Instance()->GetGraphics()->GetDeviceContext()) {
			SCR_WIDTH = LOWORD(lParam);
			SCR_HEIGHT = HIWORD(lParam);
			CRenderer::Instance()->GetGraphics()->ResizeBuffers(SCR_WIDTH, SCR_HEIGHT);
			UCameraComponent* camera = CRenderer::Instance()->GetMainCamera();
			if(camera){
				float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
				camera->UpdateRatio(aspectRatio);
			}
			if(gGuiController)
				gGuiController->Resize();
			Input::Instance()->ResizeScreen(SCR_WIDTH, SCR_HEIGHT);

		}
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WCHAR winClassName[] = L"WindowClass";
	WCHAR winTitleName[] = L"Title";

	WNDCLASS winClass = {};
	winClass.hInstance = hInstance;
	winClass.lpszClassName = winClassName;
	winClass.lpfnWndProc = WinProc;
	RegisterClass(&winClass);



	RECT rc = { 0, 0, SCR_WIDTH, SCR_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hWnd = CreateWindow(winClassName, winTitleName,
		WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		nullptr, nullptr, hInstance, nullptr
	);

	CRenderer::Instance()->Init(hWnd); // maincamera ����
	Time::Instance()->Init();
	Input::Instance()->Init(hInstance, hWnd, SCR_WIDTH, SCR_HEIGHT);

	GuiController* guiController = new GuiController(hWnd, CRenderer::Instance()->GetGraphics());
	guiController->world = new UWorld();
	gGuiController = guiController;

	UGizmoComponent* Gizmo = new UGizmoComponent();

	//Gizmo->AttachToComponent(sphere);
	// Gizmo->AttachToComponent(obj2);

	gGizmo = Gizmo;

	//worldArrow->SetRelativeScale3D({ 100,100,100 });
	//ground->SetRelativeScale3D({ 10,5,3 });
	//ground->SetRelativeLocation({ 0,-10,0 });
	//arrow->SetRelativeScale3D({ 3,3,3 });
	UCoordArrowComponent* coordArrow = new UCoordArrowComponent();
	coordArrow->SetRelativeScale3D({ 50000,50000,50000 });

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		Time::Instance()->_query_frame_update_time();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		////////////////////////////////
		// CUBE - ARROW 따라가는지 
		//if (Input::Instance()->IsKeyPressed(DIKEYBOARD_B))
		//{
		//	guiController->world->ClearWorld();
		//}
		//if (Input::Instance()->IsKeyPressed(DIKEYBOARD_N))
		//{
		//	guiController->world->SaveWorld("TestLevel");
		//	//mainScene->RemoveActor(sphere);
		//}
		//if (Input::Instance()->IsKeyPressed(DIKEYBOARD_M))
		//{
		//	guiController->world->LoadWorld("TestLevel");
		//}

		//CRenderer::Instance()->GetCamera()->PrintLoc(L"CAM");
		//obj->PrintLoc(L"obj");

		// 테스트용
		////////////////////////////////
		//Input::Instance()->Frame();
		Input::Instance()->Frame();
		//Input::Instance()->Unacquire();
		guiController->NewFrame();
		guiController->world->Update();
		CRenderer::Instance()->GetMainCamera()->allowKeyboardInput = !guiController->GetConcolWindow()->OnFocusing();
		CRenderer::Instance()->GetMainCamera()->Update();
		CRenderer::Instance()->GetMainCamera()->Render();
		CRenderer::Instance()->GetGraphics()->RenderBegin();
		guiController->world->Render();
		gGizmo->Update();
		guiController->RenderEditor();
		gGizmo->Render();
		coordArrow->Render();
		guiController->RenderFrame();
		CRenderer::Instance()->GetGraphics()->RenderEnd();
		Time::Instance()->_query_frame_end_time();
		//if (!SUCCEEDED(Input::Instance()->Acquire()))
		//	assert(0);
		
	}
	Input::Instance()->Shutdown();
	CRenderer::Release();
	return 0;
} 