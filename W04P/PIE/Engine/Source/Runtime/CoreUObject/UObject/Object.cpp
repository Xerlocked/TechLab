#include "Engine/Source/Runtime/CoreUObject/UObject/Object.h"

#include "UClass.h"
#include "UObjectHash.h"
#include "ObjectFactory.h"


UClass* UObject::StaticClass()
{
    static UClass ClassInfo{TEXT("UObject"), sizeof(UObject), alignof(UObject), nullptr};
    return &ClassInfo;
}

UObject::UObject()
    : UUID(0)
    // TODO: Object를 생성할 때 직접 설정하기
    , InternalIndex(std::numeric_limits<uint32>::max())
    , NamePrivate("None")
{
}

void UObject::DuplicateSubObjects()
{
    if (SubObjectA)
    {
        SubObjectA = SubObjectA->Duplicate();
    }
    if (SubObjectB)
    {
        SubObjectB = SubObjectB->Duplicate();
    }
}

UObject* UObject::Duplicate()
{
    UObject* NewObject = FObjectFactory::ConstructObject<UObject>();
    NewObject->DuplicateSubObjects();
    return NewObject;
}

bool UObject::IsA(const UClass* SomeBase) const
{
    const UClass* ThisClass = GetClass();
    return ThisClass->IsChildOf(SomeBase);
}
