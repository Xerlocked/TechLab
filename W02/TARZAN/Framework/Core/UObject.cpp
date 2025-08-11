#include "stdafx.h"
#include "UObject.h"

TArray<UObject*> GUObjectArray;

std::unordered_map<void*, size_t> UObject::_allocationMap = std::unordered_map<void*, size_t>();

UObject::UObject() {
	_internalIndex = GUObjectArray.size();
	GUObjectArray.push_back(this);
	_uuid = CEngineStatics::GenUUID();
}

UObject::~UObject() {
	GUObjectArray[_internalIndex] = nullptr;
}
  