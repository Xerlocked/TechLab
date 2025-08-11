#include "stdafx.h"
#include "CInputLayout.h"

TArray<D3D11_INPUT_ELEMENT_DESC> FVertexSimple::descs = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

void CInputLayout::Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, ID3DBlob* vsblob) {
	const uint32 count = static_cast<uint32>(descs.size());
	HRESULT hr = _device->CreateInputLayout(descs.data(), count, vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &_inputLayout);
}
