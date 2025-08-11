#include "stdafx.h"
#include "CState.h"

void CSamplerState::SetAddress(D3D11_TEXTURE_ADDRESS_MODE u, D3D11_TEXTURE_ADDRESS_MODE v, D3D11_TEXTURE_ADDRESS_MODE w) {
	_desc.AddressU = u;
	_desc.AddressV = v;
	_desc.AddressW = w;
}

void CSamplerState::SetBorderColor(float r, float g, float b, float a) {
	_desc.BorderColor[0] = r;
	_desc.BorderColor[1] = g;
	_desc.BorderColor[2] = b;
	_desc.BorderColor[3] = a;
}

void CSamplerState::Create() {
	_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	_desc.MaxAnisotropy = 16;
	_desc.MaxLOD = FLT_MAX;
	_desc.MinLOD = FLT_MIN;
	_desc.MipLODBias = 0.f;

	HRESULT hr = _device->CreateSamplerState(&_desc, &_samplerState);
}

void CRasterzierState::Create() {
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	//desc.CullMode = D3D11_CULL_FRONT;
	desc.FrontCounterClockwise = false;

	HRESULT hr = _device->CreateRasterizerState(&desc, &_rasterizerState);
	assert(SUCCEEDED(hr));
}

void CBlendState::SetSrcBlend(D3D11_BLEND srcBlend, D3D11_BLEND srcBlendAlpha) {
	_desc.RenderTarget[0].SrcBlend = srcBlend;
	_desc.RenderTarget[0].SrcBlendAlpha = srcBlendAlpha;
}

void CBlendState::SetDescBlend(D3D11_BLEND destBlend, D3D11_BLEND destBlendAlpha) {
	_desc.RenderTarget[0].DestBlend = destBlend;
	_desc.RenderTarget[0].DestBlendAlpha = destBlendAlpha;
}

void CBlendState::SetOp(D3D11_BLEND_OP op) {
	_desc.RenderTarget[0].BlendOp = op;
}

void CBlendState::Create() {
	_desc.RenderTarget[0].BlendEnable = true;
	_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = _device->CreateBlendState(&_desc, &_blendState);
	assert(SUCCEEDED(hr));
}