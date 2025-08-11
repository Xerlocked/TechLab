#include <windows.h>

// D3D 사용에 필요한 라이브러리들을 링크합니다.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D 사용에 필요한 헤더파일들을 포함합니다.
#include <d3d11.h>
#include <d3dcompiler.h>

// 나중에 GUI 작업시 따로 옮기기
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

#include "Renderer/URenderer.h"
#include "Editor/WildEditor.h"
#include "Scene/Scene.h"
#include "Editor/EditorDesigner.h"

// Manager
#include "Input/InputManager.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

URenderer* g_pRenderer = nullptr;

// 각종 메시지를 처리할 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
        return true;
    }

    switch (message) {
    case WM_MOUSEMOVE:
    {
        int32 x = LOWORD(lParam);
        int32 y = HIWORD(lParam);
        FInputManager::GetInst().ProcessMouseMovement(x, y);
    }
    break;

    case WM_SIZE:
    {
        UINT32 width = LOWORD(lParam);
        UINT32 height = HIWORD(lParam);
        UEditorDesigner::Get().OnResize(width, height);

        if (g_pRenderer)
        {
            g_pRenderer->OnResize(width, height);
            g_pRenderer->GetPrimaryEditor()->OnResize();
        }
    }
    break;

    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // 윈도우 클래스 이름
    WCHAR WindowClass[] = L"Wild Engine";

    // 윈도우 타이틀바에 표시될 이름
    WCHAR Title[] = L"Wild Engine";

    // 각종 메시지를 처리할 함수인 WndProc의 함수 포인터를 WindowClass 구조체에 넣는다.
    WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

    // 윈도우 클래스 등록
    RegisterClassW(&wndclass);

    // 1024 x 1024 크기에 윈도우 생성
    HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 1024,
        nullptr, nullptr, hInstance, nullptr);


    // 매니저 Class 초기화
    FInputManager::GetInst().Init();

    // Renderer Class를 생성합니다.
    URenderer* MainRender = new URenderer();

    g_pRenderer = MainRender;

    // D3D11 생성하는 함수를 호출합니다.
    MainRender->Create(hWnd);

    // Scene 생성
    UScene* MainScene = new UScene(MainRender);
    MainRender->SetPrimaryScene(MainScene);

    bool bIsExit = false;

    // FPS 제한을 위한 설정
    const int targetFPS = 60;
    const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    // 고성능 타이머 초기화
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 0.0;
    double deltaTime = 0.0;

    // Main Loop (Quit Message가 들어오기 전까지 아래 Loop를 무한히 실행하게 됨)
    while (bIsExit == false)
    {
        // 루프 시작 시간 기록
        QueryPerformanceCounter(&startTime);

        MSG msg;

        // 처리할 메시지가 더 이상 없을때 까지 수행
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&msg);

            // 메시지를 적절한 윈도우 프로시저에 전달, 메시지가 위에서 등록한 WndProc 으로 전달됨
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }
        // MainRender
        // 
        MainRender->Update(deltaTime);


        do
        {
            Sleep(0);

            // 루프 종료 시간 기록
            QueryPerformanceCounter(&endTime);

            // 한 프레임이 소요된 시간 계산 (밀리초 단위로 변환)
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;

        } while (elapsedTime < targetFrameTime);

        deltaTime = elapsedTime / 1000.0f;
    }

    MainRender->Release();

    _CrtDumpMemoryLeaks(); // 메모리 누수 체크

    return 0;
}