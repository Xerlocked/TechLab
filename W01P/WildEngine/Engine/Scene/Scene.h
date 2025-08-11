#pragma once

#include "Math/Matrix.h"
#include "Interface/IScene.h"
#include "Object/ObjectManager.h"

class URenderer;
class UObject;
class UCameraComponent;
class UCubeComponent;
class USphereComponent;
class UTriangleComponent;
class UGizmoComponent;
struct FHitResult;

class UScene : public IScene
{
public:
	UScene(URenderer* InRenderer);
	UScene(const UScene&);
	~UScene();

	void Render();
	URenderer* GetRenderer() const
	{
		return Renderer;
	};

public:
	//////////////////////
	/* IScene Interface */
	//////////////////////

	/* Selected Object */
	USceneComponent* GetSelectedObject() override;
	void SetSelectedObject(USceneComponent* newSelectObject) override;
	bool wasSelectedObject() override;

	/* Load & Save */
	void LoadScene(void* data) override;
	void SaveScene(void* data) override;

	/* Camera */
	UCameraComponent* GetPrimaryCamera() override;

	/* Construct New Object */
	void CreateNewObject(FString ObjectType, int Count) override;

	/* Gizmo */
	virtual UGizmoComponent* GetGizmo() { return SceneGizmo; };

	void OnMouseClink(int32 ScreenX, int32 ScreenY);

private:
	void Initialize();
	FMatrix CreateProjectionView();
	FMatrix CreateOrthogonalView();

	bool RayCast(FVector RayOrigin, FVector RayDirection, FHitResult& OutHitResult);

private:
	URenderer* Renderer = nullptr;
	UCameraComponent* PrimaryCamera = nullptr;
	UCubeComponent* Cube1 = nullptr;
	USphereComponent* Sphere1 = nullptr;
	UGizmoComponent* SceneGizmo = nullptr;
	UTriangleComponent* Triangle1 = nullptr;

	UCubeComponent* Cube2 = nullptr;
    TArray<UObject*>& GUObjectArray = UObjectManager::GetInst().GetObjectsArray();
	USceneComponent* SelectedObject = nullptr;

	FMatrix WorldMatrix;
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix OrthoMatrix;
};
