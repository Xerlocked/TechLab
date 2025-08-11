#include "stdafx.h"
#include "CGraphics.h"

CGraphics::CGraphics(HWND hWnd): _hWnd(hWnd) {
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
	CreateDepthStencilBuffer();
	SetViewport(_width, _height);
}

CGraphics::~CGraphics() {
	Release();
}

void CGraphics::RenderBegin() {
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, depthStencilView);
	//_deviceContext->OMSetRenderTargets(1, &_renderTargetView, nullptr);
	if (depthStencilView == nullptr)
	{
		return;
	}
	_deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	_deviceContext->ClearRenderTargetView(_renderTargetView, _clearColor);
	_deviceContext->RSSetViewports(1, &_viewPort);
}

void CGraphics::RenderEnd() {
	_swapChain->Present(1, 0);
}

void CGraphics::Release() {
	ReleaseRenderTargetView();
	ReleaseDeviceAndSwapChain();
}

void CGraphics::ResizeBuffers(int width, int height)
{
	if (_device && _deviceContext)
	{
		// 기존 참조 모두 해제
		if (_renderTargetView) { SafeRelease(&_renderTargetView); _renderTargetView = nullptr; }
		if (depthStencilView) { SafeRelease(&depthStencilView); depthStencilView = nullptr; }
		if (_backBuffer) { SafeRelease(&_backBuffer); _backBuffer = nullptr; }

		_width = width;
		_height = height;

		HRESULT hr = _swapChain->ResizeBuffers(1, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
		if (FAILED(hr))
		{
			OutputDebugString(L"ERROR: ResizeBuffers failed\n");
			return;
		}

		// 새 백 버퍼 얻기
		hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_backBuffer);
		if (FAILED(hr))
		{
			OutputDebugString(L"ERROR: GetBuffer failed\n");
			return;
		}

		// 새 렌더 타겟 뷰 생성
		hr = _device->CreateRenderTargetView(_backBuffer, nullptr, &_renderTargetView);
		if (FAILED(hr))
		{
			OutputDebugString(L"ERROR: CreateRenderTargetView failed\n");
			return;
		}

		// 새 해상도에 맞게 깊이 스텐실 버퍼 및 뷰 재생성
		CreateDepthStencilBuffer();

		// 뷰포트 업데이트
		SetViewport(width, height);
		_deviceContext->RSSetViewports(1, &_viewPort);

		// 렌더 타겟과 깊이 스텐실 뷰 설정
		_deviceContext->OMSetRenderTargets(1, &_renderTargetView, depthStencilView);

		// 새 백버퍼를 클리어하여 이전 내용 제거
		_deviceContext->ClearRenderTargetView(_renderTargetView, _clearColor);
		if (depthStencilView)
			_deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}



void CGraphics::CreateDeviceAndSwapChain() {
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC desc = {};
	desc.BufferDesc.Width = _width;
	desc.BufferDesc.Height = _height;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.SampleDesc.Count = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 1;
	desc.OutputWindow = _hWnd;
	desc.Windowed = true;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&desc,
		&_swapChain,
		&_device,
		nullptr,
		&_deviceContext
	);
	assert(SUCCEEDED(hr));
}

void CGraphics::ReleaseDeviceAndSwapChain() {
	if ( _deviceContext )
		_deviceContext->Flush();
	SafeRelease(&_renderTargetView);
	SafeRelease(&_deviceContext);
	SafeRelease(&_device);
}

void CGraphics::CreateRenderTargetView() {
	HRESULT hr;
	
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&_backBuffer);
	assert(SUCCEEDED(hr));

	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = _device->CreateRenderTargetView(_backBuffer, &desc, &_renderTargetView);
	assert(SUCCEEDED(hr));
}

void CGraphics::ReleaseRenderTargetView() {
	SafeRelease(&_backBuffer);
	SafeRelease(&_renderTargetView);
}

void CGraphics::CreateDepthStencilBuffer()
{
	HRESULT hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = _width;
	descDepth.Height = _height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = _device->CreateTexture2D(&descDepth, nullptr, &depthStencilBuffer);
	if (FAILED(hr))
		return;
		//return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = _device->CreateDepthStencilView(depthStencilBuffer, &descDSV, &depthStencilView);
	if (FAILED(hr))
		return;
		//return hr;

	//_deviceContext->OMSetRenderTargets(1, &_renderTargetView, depthStencilView);

	depthStencilBuffer->Release();
	depthStencilBuffer = nullptr;
}

void CGraphics::SetViewport(float width, float height) {
	_viewPort.TopLeftX = 0;
	_viewPort.TopLeftY = 0;
	_viewPort.Width = width;
	_viewPort.Height = height;
	_viewPort.MinDepth = 0.f;
	_viewPort.MaxDepth = 1.f;
}
