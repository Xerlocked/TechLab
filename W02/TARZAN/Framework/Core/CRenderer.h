#pragma once
#include "Framework\DirectXWrapper\CShader.h"
#include "Framework\DirectXWrapper\CInputLayout.h"
#include "Framework\DirectXWrapper\CGraphics.h"
#include "Framework\DirectXWrapper\CState.h"
#include "Framework\DirectXWrapper\CBuffer.h"
#include "./UCameraComponent.h"

class CRenderer {
private:
	static CRenderer* _instance;
	CRenderer();
	CRenderer(const CRenderer& ref) = delete;
	CRenderer& operator=(const CRenderer& ref) = delete;
	~CRenderer() {
		delete _vertexShader;
		delete _pixelShader;
		delete _graphics;
	};
public:
	static CRenderer* Instance() {
		if (_instance == nullptr)
			_instance = new CRenderer();
		return _instance;
	};
	static void Release() {
		delete _instance;
	}
	void Init(HWND hWnd);
	

	CGraphics* GetGraphics() { return _graphics; }
	void SetVertexShader(const FWString filename, FString funcname, FString version);
	void ResetVertexShader();
	void SetPixelShader(const FWString filename, FString funcname, FString version);
	void ResetPixelShader();
	void SetRasterzierState();
	void SetTransformToConstantBuffer(FMatrix matrix);
	void SetFlagsToConstantBuffer(FPrimitiveFlags flags);
	UCameraComponent* GetMainCamera() const;
	void SetMainCamera(UCameraComponent* camera);
private:
	CGraphics* _graphics = nullptr;
	CVertexShader* _vertexShader = nullptr;
	CPixelShader* _pixelShader = nullptr;
	CInputLayout* _inputLayout = nullptr;
	CRasterzierState* _rasterizerState = nullptr;
	CConstantBuffer<FMatrix>* _matrixBuffer = nullptr;
	CConstantBuffer<FPrimitiveFlags>* _flagsBuffer = nullptr;
	UCameraComponent* _mainCamera = nullptr;
};

