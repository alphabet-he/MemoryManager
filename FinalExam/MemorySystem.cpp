#include "MemorySystem.h"
#include <cassert>

HeapManager* p_HeapManager = nullptr;

bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors)
{
	// create your HeapManager and FixedSizeAllocators

	// create dynamic heap manager that allocates finxed size allocators
	HeapManager* DynamicHeapManager = HeapManagerProxy::CreateHeapManager(i_pHeapMemory, i_sizeHeapMemory, i_OptionalNumDescriptors);
	assert(DynamicHeapManager);
	p_HeapManager = DynamicHeapManager;
	
	// create fixed size allocators by memory system params
	int FSACount = sizeof(FixedSizes) / sizeof(FixedSizes[0]);

	for (int i = 0; i < FSACount; i++) {

		// ideal size and block nums
		size_t FSAsize = i_sizeHeapMemory * FixedSizedRatio * SizeRatios[i];
		unsigned long blockNum = FSAsize / FixedSizes[i];

		// make sure the block num is multiply of 8
		blockNum = blockNum - blockNum % 8;
		
		// alocate the fixed size allocator by dynamic heap manager
		FixedSizeAllocator* FSA = HeapManagerProxy::CreateFixedSizeAllocator(DynamicHeapManager, FixedSizes[i], blockNum);
		
		// no enough space to allocate
		if (!FSA) return false;
	}

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
	HeapManagerProxy::Destroy(p_HeapManager);
	p_HeapManager = nullptr;
}

FixedSizeAllocator* ExpandFixedSizeAllocator(unsigned long fixedSize)
{
	// find fixed size ratio
	int ind = -1;
	int FSACount = sizeof(FixedSizes) / sizeof(FixedSizes[0]);
	for (int i = 0; i < FSACount; i++) {
		if (fixedSize == FixedSizes[i]) {
			ind = i;
			break;
		}
	}
	if (ind < 0) return nullptr;

	// ideal size and block nums
	size_t FSAsize = p_HeapManager->sizeHeap * FixedSizedRatio * SizeRatios[ind];
	unsigned long blockNum = FSAsize / fixedSize;

	// make sure the block num is multiply of 8
	blockNum = blockNum - blockNum % 8;

	FixedSizeAllocator* FSA = HeapManagerProxy::CreateFixedSizeAllocator(p_HeapManager, fixedSize, blockNum);

	// if there is enough space to expand it twice as large, return true
	if (FSA) return FSA;
	// if there is no enough space to have another as large FSA,
	// try enlarge it to 1.5 times as large
	blockNum /= 2;
	blockNum = blockNum - blockNum % 8;
	return HeapManagerProxy::CreateFixedSizeAllocator(p_HeapManager, fixedSize, blockNum);
}


