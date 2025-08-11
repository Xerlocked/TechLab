#include "Object.h"
#include "Object/ObjectManager.h"

UObject::UObject()
{    // Manager에 객체 등록
    UObjectManager::GetInst().RegisterObject(this);
}

UObject::~UObject()
{
}

UClass* UObject::GetClass()
{
    static UClass ObjectClass("UObjectComponent", nullptr);
    return &ObjectClass;
}

void* UObject::operator new(size_t Size)
{
    void* Ptr = ::operator new(Size);
    UObject* NewObj = static_cast<UObject*>(Ptr);

    UObjectManager::GetInst().RegisterAllocation(NewObj, Size);

    return Ptr;
}

void UObject::operator delete(void* Ptr)
{
    if (!Ptr) return;

    UObject* Obj = static_cast<UObject*>(Ptr);
    UObjectManager::GetInst().UnregisterObject(Obj);
    UObjectManager::GetInst().RegisterDeallocation(Obj);


    ::operator delete(Ptr);
}
