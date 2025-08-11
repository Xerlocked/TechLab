#pragma once
#include "stdafx.h"
#include "Framework/Core/CEngineStatics.h"
#include "Framework/Core/CObjectMeta.h"
#include <unordered_map>

class UObject {
private:
	uint32 _uuid;
	uint32 _internalIndex;		// ???
	static std::unordered_map<void*, size_t> _allocationMap;
public:
	UObject();
	virtual ~UObject();

	void* operator new(size_t size) {
		void* ptr = ::operator new(size);
		++CEngineStatics::TotalAllocationCount;
		_allocationMap[ptr] = size;
		CEngineStatics::TotalAllocationBytes += static_cast<uint32>(size);
		return ptr;
	}

	void operator delete(void* p) {
		--CEngineStatics::TotalAllocationCount;
		CEngineStatics::TotalAllocationBytes -= static_cast<uint32>(_allocationMap[p]);
		_allocationMap.erase(p);
		free(p);
	}

	inline bool operator==(const UObject& rhs) const {
		return _uuid == rhs._uuid;
	}

	uint32 GetUUID() { return _uuid; }
	uint32 GetId() { return _internalIndex; }

	
};

// TArray		순회 가능, 상수시간에 접근, 메모리 파편화
// TLinkedList	순회 가능, On시간에 접근, 메모리 절약
// TMap			순회 가능, 상수시간에 접근
// Map을 직접 만들란 소린가?