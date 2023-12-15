#include "MemorySystem.h"
#include "HeapManagerProxy.h"
#include <cassert>

bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors)
{
	// create your HeapManager and FixedSizeAllocators

	// create dynamic heap manager that allocates finxed size allocators
	HeapManager* DynamicHeapManager = HeapManagerProxy::CreateHeapManager(i_pHeapMemory, i_sizeHeapMemory, i_OptionalNumDescriptors);
	assert(DynamicHeapManager);

	return true;
}

void Collect()
{
	// coalesce free blocks
	// you may or may not need to do this depending on how you've implemented your HeapManager
}

void DestroyMemorySystem()
{
	// Destroy your HeapManager and FixedSizeAllocators
}

