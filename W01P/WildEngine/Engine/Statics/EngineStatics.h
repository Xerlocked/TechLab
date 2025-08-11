#pragma once
#include "Types/Types.h"

class UEngineStatics {
public:
	static uint32 GenUUID();

private:
	static uint32 NextUUID;
};