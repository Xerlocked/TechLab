#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Math/Matrix.h"

struct FVertexType;

class UScene;
class UWildEditor;

// 렌더러 클래스 선언
class URenderer
{
public:
    // Direct3D 장치 및 관련 객체 포인터
    ID3D11Device* Device = nullptr;                   // Direct3D 장치
    ID3D11DeviceContext* DeviceContext = nullptr;       // 장치 컨텍스트
    IDXGISwapChain* SwapChain = nullptr;                // 스왑 체인

    // 렌더링 리소스 및 상태
    ID3D11Texture2D* FrameBuffer = nullptr;             // 백 버퍼 텍스처
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;     // 렌더 타겟 뷰
    ID3D11RasterizerState* RasterizerState = nullptr;     // 래스터라이저 상태
    ID3D11DepthStencilState* DepthStencilState = nullptr; // 스탠실 상태
    ID3D11DepthStencilState* GizmoDepthStencilState = nullptr; // Gizmo 스탠실 상태
    ID3D11Buffer* MatrixBuffer = nullptr;             // 상수 버퍼

    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };  // 초기 클리어 색상 (RGBA)
    D3D11_VIEWPORT ViewportInfo;                        // 뷰포트 정보

    // 쉐이더 관련 멤버
    ID3D11VertexShader* SimpleVertexShader = nullptr;
    ID3D11PixelShader* SimplePixelShader = nullptr;
    ID3D11InputLayout* SimpleInputLayout = nullptr;
    unsigned int Stride = 0;


    ID3D11DepthStencilView* DepthStencilView = nullptr;

public:
    // 생성 및 해제 관련 함수
    void Create(HWND hWindow);
    void Release();

    // 업데이트 함수
    void Update(float deltaTime);

    // 상수 버퍼 업데이트
    void UpdateShaderParameters(FMatrix World, FMatrix View, FMatrix Projection);

    // 상수 버퍼 생성
    ID3D11Buffer* CreateVertexBuffer(FVertexType* vertices, UINT byteWidth);

    // 그리기
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices);

    void RenderGizmo(ID3D11Buffer* pBuffer, UINT numVertices);

    void SetPrimaryScene(UScene* NewScene);

    UScene* GetPrimaryScene() { return PrimaryScene; }

    UWildEditor* GetPrimaryEditor() { return PrimaryEditor; }

    void OnResize(uint32 Width, uint32 Height);
private:
    // 렌더링 처리 함수
    void SwapBuffer();
    void Prepare();
    void PrepareShader();

    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer);

    // 상수 버퍼 관련 함수
    void CreateMatrixBuffer();
    void ReleaseMatrixBuffer();

    // 쉐이더 생성/해제 함수
    void CreateShader();
    void ReleaseShader();

    // 내부적으로 사용되는 Direct3D 장치 및 스왑 체인 관리 함수
    void CreateDeviceAndSwapChain(HWND hWindow);
    void ReleaseDeviceAndSwapChain();

    // 프레임 버퍼 및 래스터라이저 상태 관리 함수
    void CreateFrameBuffer();
    void ReleaseFrameBuffer();
    
    // 레스터라이저 생성
    void CreateRasterizerState();
    void ReleaseRasterizerState();

    // 스텐실 생성
    void CreateStencilState();
    void CreateStencilBuffer();
    void ReleaseStencilState();
    void ReleaseStencilBuffer();

private:
    UScene* PrimaryScene;

    UWildEditor* PrimaryEditor;

    struct FMatrixType
    {
        FMatrix MVP;
    };

};