#pragma once

template <typename T>
class CBuffer {
public:
	CBuffer(ID3D11Device* device) : _device(device) {};
	virtual ~CBuffer() {
		SafeRelease(&_buffer);
	};

	ID3D11Buffer* Get() { return _buffer; }
	UINT32 GetStride() { return _stride; }
	UINT32 GetOffset() { return _offset; }
	UINT32 GetCount() { return _count; }

	virtual void Create(const std::vector<T>&) abstract;

protected:
	ID3D11Device* _device;
	ID3D11Buffer* _buffer = nullptr;

	UINT32 _stride = 0;
	UINT32 _offset = 0;
	UINT32 _count = 0;
};


template <typename T>
class CVertexBuffer : public CBuffer<T> {
public:
	using Super = CBuffer<T>;
	CVertexBuffer(ID3D11Device* device) : Super(device) {};
	~CVertexBuffer() {};

	void Create(const std::vector<T>& vertices) override;
};

template <typename T>
inline void CVertexBuffer<T>::Create(const std::vector<T>& vertices) {
	this->_stride = sizeof(T);
	this->_count = static_cast<UINT32>(vertices.size());

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_IMMUTABLE;			// immutable: gpu가 읽기 전용으로 접근할 수 있다.
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// vertex buffer로 사용하겠다.
	desc.ByteWidth = (UINT32)(this->_stride * this->_count);	// buffer 크기 지정

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();

	HRESULT hr = this->_device->CreateBuffer(&desc, &data, &this->_buffer);
	assert(SUCCEEDED(hr));
}


class CIndexBuffer : public CBuffer<UINT32> {
public:
	using Super = CBuffer<UINT32>;
	CIndexBuffer(ID3D11Device* device) : Super(device) {};
	~CIndexBuffer() {};
	void Create(const std::vector<UINT32>& indices) override;
};

inline void CIndexBuffer::Create(const std::vector<UINT32>& indices) {
	this->_stride = sizeof(UINT32);
	this->_offset = 0;
	this->_count = static_cast<UINT32>(indices.size());

	D3D11_BUFFER_DESC desc = {};						// buffer의 종류, 용도 등을 지정
	desc.Usage = D3D11_USAGE_IMMUTABLE;			// immutable: gpu가 읽기 전용으로 접근할 수 있다.
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// index buffer로 사용하겠다.
	desc.ByteWidth = (UINT32)(sizeof(UINT32) * this->_count);	// buffer 크기 지정

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indices.data();

	HRESULT hr = this->_device->CreateBuffer(&desc, &data, &this->_buffer);
}


template <typename T>
class CConstantBuffer : public CBuffer<T> {
public:
	using Super = CBuffer<T>;
	CConstantBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : Super(device), _deviceContext(deviceContext) {};
	~CConstantBuffer() {};

	void Create(const std::vector<T>&);
	void Create();
	void CopyData(const T& data);

protected:
	ID3D11DeviceContext* _deviceContext;
};

template<typename T>
inline void CConstantBuffer<T>::Create(const std::vector<T>&) {
	this->_stride = sizeof(T);
	this->_offset = 0;
	this->_count = 0;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = (this->_stride + 15) & ~15; // 16의 배수 보정
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = this->_device->CreateBuffer(&desc, nullptr, &this->_buffer);
}

template<typename T>
inline void CConstantBuffer<T>::Create() {
	this->_stride = sizeof(T);
	this->_offset = 0;
	this->_count = 0;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = (this->_stride + 15) & ~15; // 16의 배수 보정
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = this->_device->CreateBuffer(&desc, nullptr, &this->_buffer);
}

template<typename T>
inline void CConstantBuffer<T>::CopyData(const T& data) {
	D3D11_MAPPED_SUBRESOURCE subResources = {};

	this->_deviceContext->Map(this->_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResources);
	memcpy(subResources.pData, &data, sizeof(data));
	this->_deviceContext->Unmap(this->_buffer, 0);
}
