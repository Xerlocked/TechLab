#pragma once
#include "Framework/Core/UPrimitiveComponent.h"
static const FVector AxisColors[] = {
	FVector(1.0f, 0.0f, 0.0f),  // X��: ����
	FVector(0.0f, 1.0f, 0.0f),  // Y��: �ʷ�
	FVector(0.0f, 0.0f, 1.0f)   // Z��: �Ķ�
};

class UArrowComponent:public UPrimitiveComponent
{
public:
	UArrowComponent(EPrimitiveColor axis);
	~UArrowComponent();
	virtual void Render();
	bool IsPicked() { return picked; }
	void SetPicked(bool pik) { picked = pik; }
private:
	//for boundBox
	FVector minPos;
	FVector maxPos;
	// picking
	bool picked = false;
	EPrimitiveColor color;
	//static ID3D11DepthStencilState* noDepthWriteState;
	//static ID3D11DepthStencilState* previousWriteState;
};

