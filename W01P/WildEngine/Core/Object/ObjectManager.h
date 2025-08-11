#pragma once
#include "Object/Object.h"
#include "Log/DebugConsole.h"
#include "Statics/EngineStatics.h"
#include "Interface/ISingleton.h"

class UObjectManager : public ISingleton<UObjectManager> 
{
public:
    void RegisterObject(UObject* Object);
    void UnregisterObject(UObject* Object);

    void RegisterAllocation(UObject* Object, size_t Size);
    void RegisterDeallocation(UObject* Object);

    void PrintMemoryUsage();

    uint32 GetTotalAllocationBytes() { return TotalAllocationBytes; };
    uint32 GetTotalAllocationCount() { return TotalAllocationCount; };

    template <typename T>
    TArray<T*> GetObjectsOfType();

    TArray<UObject*>& GetObjectsArray() { return GUObjectArray; }
	uint32 GetNextUUID() { return NextUUID; }

private:
    friend class ISingleton<UObjectManager>;
    UObjectManager() = default;

    TArray<UObject*> GUObjectArray;

    TMap<void*, size_t> AllocationMap;

    // 메모리 사용량 추적
    uint32 TotalAllocationBytes = 0;
    uint32 TotalAllocationCount = 0;

    uint32 NextUUID = 1;
};

template<typename T>
inline TArray<T*> UObjectManager::GetObjectsOfType()
{
    TArray<T*> result;
    for (UObject* obj : GUObjectArray)
    {
        T* casted = dynamic_cast<T*>(obj);
        if (casted)
            result.push_back(casted);
    }
    return result;
}

