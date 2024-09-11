#pragma once
#include "MemoryBlockHeader.h"

struct HeapManager
{
	void* pHeapMemory; // actual memory starts from (not count the Manager info)
	size_t sizeHeap; // actual memory that can use (not count the Manager info)
	MemoryBlockHeader* FreeList;
	unsigned long numDescriptors;
};

const size_t _HeapManagerSize = sizeof(HeapManager);

