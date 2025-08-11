#pragma once

class USceneComponent;
class UCameraComponent;
class UGizmoComponent;

class IScene
{
public:
	
	/* Selected Object */
	virtual USceneComponent* GetSelectedObject() = 0;
	virtual void SetSelectedObject(USceneComponent*) = 0;
	virtual bool wasSelectedObject() = 0;

	/* Load & Save */
	virtual void LoadScene(void* data) = 0;
	virtual void SaveScene(void* data) = 0;

	/* Camera */
	virtual UCameraComponent* GetPrimaryCamera() = 0;

	/* Construct New Object */
	virtual void CreateNewObject(FString, int) = 0;

	/* Gizmo */
	virtual UGizmoComponent* GetGizmo() = 0;
};