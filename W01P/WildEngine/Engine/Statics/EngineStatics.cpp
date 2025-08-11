#include "EngineStatics.h"

uint32 UEngineStatics::NextUUID = 1;

uint32 UEngineStatics::GenUUID()
{
	return NextUUID++;
}
