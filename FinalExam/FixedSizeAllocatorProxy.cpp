#include "FixedSizeAllocatorProxy.h"
#include "FixedSizeAllocator.h"
#include "MemoryBlockHeader.h"
#include <cassert>
#include "WriteMemory.h"

FixedSizeAllocator* FixedSizeAllocatorProxy::CreateFixedSizeAllocator(void* baseAddr, int fixedSize, int blockNum)
{
	return nullptr;
}

void* FixedSizeAllocatorProxy::alloc(FixedSizeAllocator* pFixedSizeAllocator, size_t sizeAlloc)
{
	return nullptr;
}

bool FixedSizeAllocatorProxy::free(FixedSizeAllocator* pFixedSizeAllocator, void* ptr)
{
	return false;
}

bool FixedSizeAllocatorProxy::Contains(void* ptr)
{
	return false;
}

void FixedSizeAllocatorProxy::Destroy(FixedSizeAllocator* pFixedSizeAllocator)
{
}
