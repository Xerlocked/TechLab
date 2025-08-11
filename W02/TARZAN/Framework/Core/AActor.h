#pragma once
#include "Framework/Core/UObject.h"
#include "Framework/Core/UActorComponent.h"

class AActor : public UObject {
private:
	TLinkedList<UActorComponent*> _components;

public:
	AActor();
	virtual ~AActor();
	void OnRegister(UActorComponent* component);
	void OnUnregister(UActorComponent* component);
	virtual void Update();
	virtual void Render();
};