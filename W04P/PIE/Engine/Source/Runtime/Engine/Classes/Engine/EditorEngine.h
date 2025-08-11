#pragma once

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

struct FWorldContext
{

};

class UEditorEngine : public UObject
{
    DECLARE_CLASS(UEditorEngine, UObject)

public:
    //TArray<FWorldContext> WorldContexts;

    virtual void Tick(float DeltaSeconds);
};

