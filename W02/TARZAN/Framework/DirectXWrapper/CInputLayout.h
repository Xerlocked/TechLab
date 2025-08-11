#pragma once
class CInputLayout {
public:
	CInputLayout(ID3D11Device* device) : _device(device), _inputLayout(nullptr) {};
	~CInputLayout() { 
		SafeRelease(&_inputLayout); 
	};
	void Release() {};
	ID3D11InputLayout* Get() { return _inputLayout; }

	void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, ID3DBlob* vsblob);

private:
	ID3D11Device* _device;
	ID3D11InputLayout* _inputLayout;
};

struct FVertexSimple {
	float x, y, z;
	float r, g, b, a;
	static TArray<D3D11_INPUT_ELEMENT_DESC> descs;
};