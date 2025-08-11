#pragma once

#include "Gizmo.h"
#include "SceneComponent.h"

class URenderer;
enum class EGizmoType;

class UGizmoComponent : public USceneComponent
{
public:
	UGizmoComponent(URenderer* InRenderer);
	UGizmoComponent(const UGizmoComponent&) = delete;
	~UGizmoComponent();

	static UClass* GetClass();
	UClass* GetInstanceClass() const override;

	void Render(FMatrix WorldMatrix, FMatrix ViewMatrix, FMatrix ProjectionMatrix);

	void SetGizmoType(EGizmoType NewType);

	EGizmoType GetCurrentGizmo() { return CurrentType; }
private:
	URenderer* Renderer;

	EGizmoType CurrentType;
	
	UINT NumVerticesX;
	ID3D11Buffer* VertexBufferX;

	UINT NumVerticesY;
	ID3D11Buffer* VertexBufferY;

	UINT NumVerticesZ;
	ID3D11Buffer* VertexBufferZ;
};

