#include "stdafx.h"
#include "UArrowComponent.h"

//#include "CRenderer.h"

ID3D11DepthStencilState* noDepthWriteState;
ID3D11DepthStencilState* previousWriteState;

UArrowComponent::UArrowComponent(EPrimitiveColor axis) : UPrimitiveComponent(), color(axis)
{
	std::wstring filepath = L"Assets/AxisArrow.obj";
	std::ifstream objFile(filepath.c_str());
	FVector color = AxisColors[axis];


	if (objFile) {

		std::string line;
		std::vector<FVector> positions;

		while (std::getline(objFile, line)) {
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;

			if (type == "v") // Vertex position
			{
				FVector vertex;
				lineStream >> vertex.x >> vertex.y >> vertex.z;
				FVertexSimple vertexSimple{ vertex.x, vertex.y, vertex.z, color.x,color.y,color.z, 1 };
				vertices.push_back(vertexSimple);
				positions.push_back(vertex);
			}
			else if (type == "f") // Face
			{
				std::vector<uint32_t> faceIndices;
				uint32_t index;

				while (lineStream >> index) {
					faceIndices.push_back(index - 1);
				}

				for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
					indices.push_back(faceIndices[0]);    // ù ��° ����
					indices.push_back(faceIndices[i]);    // �� ��° ����
					indices.push_back(faceIndices[i + 1]); // �� ��° ����
				}
			}

		}
	}
	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	_vertexBuffer = new CVertexBuffer<FVertexSimple>(graphics->GetDevice());
	_vertexBuffer->Create(vertices);
	_indexBuffer = new CIndexBuffer(graphics->GetDevice());
	_indexBuffer->Create(indices);
}


UArrowComponent::~UArrowComponent()
{
}

void UArrowComponent::Render()
{ 

	//switch (color)
	//{
	//case RED_X:
	//	OverrideRotation = { 0,-M_PI / 2,0 };
	//	break;
	//case GREEN_Y:
	//	OverrideRotation = { M_PI / 2 ,0,0 };
	//	break;
	//case BLUE_Z:
	//	OverrideRotation = { 0,0,0 };
	//	break;
	//}
	//OverrideLocation = GetAttachParent()->GetComponentLocation();
	//OverrideScale3D = GetAttachParent()->GetRelativeScale3D();
 	UPrimitiveComponent::Render();
	
	return;


	//auto d = CRenderer::Instance()->GetGraphics()->GetDevice();
	//auto dc = CRenderer::Instance()->GetGraphics()->GetDeviceContext();
	//// depth test bypass용 버퍼
	//if (noDepthWriteState == nullptr)
	//{
	//	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	//	depthStencilDesc.DepthEnable = true;
	//	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 쓰기 X
	//	d->CreateDepthStencilState(&depthStencilDesc, &noDepthWriteState);
	//}

	//static UINT stencilRef;
	//if (color == RED_X)
	//{
	//	// 이전의 depth stencil state를 저장
	//	dc->OMGetDepthStencilState(&previousWriteState, &stencilRef);

	//	// 새로운 depth stencil state 지정
	//	dc->OMSetDepthStencilState(noDepthWriteState, 1);
	//}


	//UPrimitiveComponent::Render();


	//if (color = BLUE_Z)
	//{
	//	// depth stencil 원래대로 복원
	//	dc->OMSetDepthStencilState(previousWriteState, stencilRef);
	//}

	//////////////////////////
	///////////////////////////

	//static bool _is_first_called = false;
	//static ID3D11DepthStencilState* depthAlwaysState = nullptr;

	//// 처음 호출시 override하는 뎁스 생성
	//if (!_is_first_called)
	//{
	//	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	//	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	//	depthDesc.DepthEnable = TRUE;  // Depth Test 활성화
	//	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;  // 항상 통과
	//	depthDesc.StencilEnable = FALSE;  // Stencil 사용 X
	//	CRenderer::Instance()->GetGraphics()->GetDevice()->CreateDepthStencilState(&depthDesc, &depthAlwaysState);

	//	_is_first_called = true;
	//}

	//static ID3D11DepthStencilState* prevDepthState = nullptr;
	//static UINT prevStencilRef;

	//if (color == RED_X)
	//{
	//	CRenderer::Instance()->GetGraphics()->GetDeviceContext()->OMGetDepthStencilState(&prevDepthState, &prevStencilRef);
	//}
	////CRenderer::Instance()->GetGraphics()->GetDeviceContext()->ClearDepthStencilView(CRenderer::Instance()->GetGraphics()->d, D3D11_CLEAR_DEPTH, 1.0f, 0);

	////deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	////CRenderer::Instance()->GetGraphics()->GetDeviceContext()->OMSetDepthStencilState(depthAlwaysState, 1);

	//ID3D11DepthStencilView* currentDSV = nullptr;
	//ID3D11RenderTargetView* currentRTV = nullptr;

	//// 현재 Render Target과 Depth Stencil View를 가져옴
	//CRenderer::Instance()->GetGraphics()->GetDeviceContext()->OMGetRenderTargets(1, &currentRTV, &currentDSV);

	//CRenderer::Instance()->GetGraphics()->GetDeviceContext()->ClearDepthStencilView(currentDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	///////////////////////////


	
	//switch (color)
	//{
	//case RED_X:
	//	OverrideRotation = { 0,-M_PI / 2,0 };
	//	break;
	//case GREEN_Y:
	//	OverrideRotation = { M_PI / 2 ,0,0 };
	//	break;
	//case BLUE_Z:
	//	OverrideRotation = { 0,0,0 };
	//	break;
	//}
	//OverrideLocation = GetAttachParent()->GetComponentLocation();
	//OverrideScale3D = GetAttachParent()->GetRelativeScale3D();
	//UPrimitiveComponent::Render();

	//if (color == BLUE_Z)
	//{
	//	//CRenderer::Instance()->GetGraphics()->GetDeviceContext()->OMSetDepthStencilState(prevDepthState, prevStencilRef);
	//	SafeRelease(&prevDepthState);

	//}

}

