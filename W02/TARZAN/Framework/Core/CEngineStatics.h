#pragma once
class CEngineStatics {
private:
	static uint32 _nextUUID;

public:
	static uint32 GenUUID() {
		return _nextUUID++;
	}
	static uint32 TotalAllocationBytes;
	static uint32 TotalAllocationCount;
};

