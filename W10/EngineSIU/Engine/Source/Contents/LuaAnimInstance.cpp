#include "LuaAnimInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

void ULuaAnimInstance::UpdateAnimation(float DeltaSeconds)
{
    UAnimInstance::UpdateAnimation(DeltaSeconds);
    
    bIsWalking = GEngineLoop.bIsWalking;
    bIsJumping = GEngineLoop.bIsJumping;
}

void ULuaAnimInstance::SetLuaFunction()
{
    UAnimInstance::SetLuaFunction();

    LuaState.set_function("IsWalking", [this]() { return this->bIsWalking; });
    LuaState.set_function("IsJumping", [this]() { return this->bIsJumping; });
}
