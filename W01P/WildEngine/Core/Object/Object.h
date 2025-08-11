#pragma once
#include "Types/Types.h"
#include "Class/Class.h"

class UObject
{
public:
	UObject();
	virtual ~UObject();

	uint32 UUID;
	uint32 InternalIndex; // Index of GUObjectArray

	// 정적 클래스 정보 반환
	static UClass* GetClass();

	// 객체의 동적 클래스 정보 반환
	virtual UClass* GetInstanceClass() const { return GetClass(); }

	// 특정 클래스인지 확인
	bool IsA(UClass* ClassType) const {
		return GetInstanceClass()->IsChildOf(ClassType);
	}

	// new 연산자 오버로딩
	void* operator new(size_t size);
	void operator delete(void* ptr);
};