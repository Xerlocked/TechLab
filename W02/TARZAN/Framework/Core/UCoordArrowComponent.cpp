//#include "UCoordArrowComponent.h"
//
//
//UCoordArrowComponent::UCoordArrowComponent()
//{
//	static const vertices =
//	{
//		{0,0,0,1,0,0,1},
//		{1,0,0,1,0,0,1},
//		{0,1,0,0,1,0,1},
//		{0,1,0,0,1,0,1},
//		{0,0,1,0,0,1,1},
//		{0,0,1,0,0,1,1},
//	}
//}
//
//UCoordArrowComponent::~UCoordArrowComponent()
//{
//}
//
//void UCoordArrowComponent::Render()
//{
//
//}
//
//void UCoordArrowComponent::Update()
//{
//}
//

#include "stdafx.h"
#include "UCoordArrowComponent.h"
#include "./Framework/Core/CRenderer.h"

UCoordArrowComponent::UCoordArrowComponent()
{
	vertices =
	{
		{ 0.f, 0.f, 0.f, 5.f, 0.f, 0.f, 5.f},
		{ 5.f, 0.f, 0.f, 5.f, 0.f, 0.f, 5.f},
		{ 0.f, 0.f, 0.f, 0.f, 5.f, 0.f, 5.f},
		{ 0.f, 5.f, 0.f, 0.f, 5.f, 0.f, 5.f},
		{ 0.f, 0.f, 0.f, 0.f, 0.f, 5.f, 5.f},
		{ 0.f, 0.f, 5.f, 0.f, 0.f, 5.f, 5.f},
	};

	indices = { 0,1,2,3,4,5 };

	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = new CIndexBuffer(graphics->GetDevice());
	_indexBuffer->Create(indices);
}

UCoordArrowComponent::~UCoordArrowComponent()
{
	delete _vertexBuffer;
	delete _indexBuffer;
}

void UCoordArrowComponent::Update()
{
}

void UCoordArrowComponent::Set(FVector origin, FVector direction)
{
	if (OriginWhenSpawned == FVector(0, 0, 0) && DirectionWhenSpawned == FVector(0, 0, 0))
	{
		OriginWhenSpawned = origin;
		DirectionWhenSpawned = direction;
	}
	else
	{
		// 여기 오면 안됨
	}
}

FVector UCoordArrowComponent::GetOrigin()
{
	return OriginWhenSpawned;
}

FVector UCoordArrowComponent::GetDirection()
{
	return DirectionWhenSpawned;
}

void UCoordArrowComponent::Render()
{
	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	UPrimitiveComponent::Render();
	//CRenderer::Instance()->SetRasterzierState();
	//ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
	//uint32 stride = _vertexBuffer->GetStride();
	//uint32 offset = _vertexBuffer->GetOffset();
	//graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);
	//FMatrix m = GetComponentTransform();
	//CRenderer::Instance()->SetConstantBuffer(m);
	//graphics->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
	graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
