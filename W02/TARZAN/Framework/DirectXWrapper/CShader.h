#pragma once
class CShader {
public:
	CShader(ID3D11Device* device) : _device(device), _blob(nullptr) {};
	virtual ~CShader() {
		SafeRelease(&_blob);
	};
	void Release();

	virtual void Create(const FWString& path, const FString& name, const FString& version) abstract;
	ID3DBlob* GetBlob() { return _blob; }

protected:
	void LoadShaderFromFile(const FWString& path, const FString& name, const FString& version);

protected:
	FWString _path;
	FString _name;
	ID3D11Device* _device;
	ID3DBlob* _blob;
};

class CVertexShader : public CShader {
	using Super = CShader;
public:
	CVertexShader(ID3D11Device* device) : Super(device), _vertexShader(nullptr) {};
	~CVertexShader() {
		SafeRelease(&_vertexShader);
	};

	void Create(const FWString& path, const FString& name, const FString& version) override;
	ID3D11VertexShader* Get() { return _vertexShader; }

private:
	ID3D11VertexShader* _vertexShader;
};

class CPixelShader : public CShader {
	using Super = CShader;
public:
	CPixelShader(ID3D11Device* device) : Super(device), _pixelShader(nullptr) {};
	~CPixelShader() {
		SafeRelease(&_pixelShader);
	};

	void Create(const FWString& path, const FString& name, const FString& version) override;
	ID3D11PixelShader* Get() { return _pixelShader; }

private:
	ID3D11PixelShader* _pixelShader;
};
