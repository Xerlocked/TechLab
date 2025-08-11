#include "stdafx.h"
#include "UPlaneComponent.h"
#include "./Framework/Core/CRenderer.h"

UPlaneComponent::UPlaneComponent() {
	vertices = 
	{
		//   x,   y,   z       r,   g,   b,   a
		{-1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f }, 
		{-1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f }
	};
	
	indices = 
	{
		0, 2, 1,  
		0, 3, 2
	};

	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = new CIndexBuffer(graphics->GetDevice());
	_indexBuffer->Create(indices);
}