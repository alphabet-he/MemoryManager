#include <inttypes.h>
#include <stdio.h>
#include "MemorySystem.h"
#include "HeapManagerProxy.h"
#include "FixedSizeAllocator.h"

void* __cdecl malloc(size_t i_size)
{
	// replace with calls to your HeapManager or FixedSizeAllocators
	printf("malloc %zu\n", i_size);
	
	// determine whether it belongs to FSA
	int FSACount = sizeof(FixedSizes) / sizeof(FixedSizes[0]);
	// if it larger than the largest size
	if (i_size > FixedSizes[FSACount - 1]) {
		// dynamic heap manager will allocate it
		return HeapManagerProxy::alloc(p_HeapManager, i_size);
	}
	// the size can be allocated by FSAs
	else {
		
		// find out what size it belongs
		size_t targetSize;
		for (int i = 0; i < FSACount; i++)
		{
			if (FixedSizes[i] >= i_size) 
			{
				targetSize = FixedSizes[i];
				break;
			}
		}

		// find the FSA of target size
		MemoryBlockHeader* currBlock = (MemoryBlockHeader*)(p_HeapManager->pHeapMemory);
		while (currBlock) {
			if (currBlock->fixedSized) {
				FixedSizeAllocator* FSA = (FixedSizeAllocator*)(currBlock->pBaseAddr);
				if (FSA->GetFixedSize() == targetSize && !FSA->AreAllUsed()) break;
				currBlock = currBlock->pNextBlock;
			}
		}

		// if the FSA can allocate the size
		if (currBlock) {
			FixedSizeAllocator* FSA = (FixedSizeAllocator*)(currBlock->pBaseAddr);
			return FSA->alloc();
		}

		// if the FSA is full
		else
		{
			// try expand FSA of target size
			FixedSizeAllocator* tempFSA = ExpandFixedSizeAllocator(targetSize);
			// if the expansion succeeds
			if (tempFSA) {
				return tempFSA->alloc();
			}
			// if there is no enough space to expand the FSA,
			// allocate it by dynamic heap manager
			else
			{
				return HeapManagerProxy::alloc(p_HeapManager, i_size);
			}
		}
	}

	return nullptr;
}

void __cdecl free(void* i_ptr)
{
	// replace with calls to your HeapManager or FixedSizeAllocators
	printf("free 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));

	// traverse FSAs to determine the one that allocates i_ptr
	MemoryBlockHeader* currBlock = (MemoryBlockHeader*)(p_HeapManager->pHeapMemory);
	while (currBlock) {
		if (currBlock->fixedSized) {
			FixedSizeAllocator* FSA = (FixedSizeAllocator*)(currBlock->pBaseAddr);
			if (FSA->Contains(i_ptr)) {
				FSA->free(i_ptr);
				return;
			}
		}
		currBlock = currBlock->pNextBlock;
	}
	HeapManagerProxy::free(p_HeapManager, i_ptr);
	return;
	//return _aligned_free(i_ptr);
}

void* operator new(size_t i_size)
{
	// replace with calls to your HeapManager or FixedSizeAllocators
	printf("new %zu\n", i_size);
	return malloc(i_size);
	//return _aligned_malloc(i_size, 4);
}

void operator delete(void* i_ptr)
{
	// replace with calls to your HeapManager or FixedSizeAllocators
	printf("delete 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));
	return free(i_ptr);
	//return _aligned_free(i_ptr);
}

void* operator new[](size_t i_size)
{
	// replace with calls to your HeapManager or FixedSizeAllocators
	printf("new [] %zu\n", i_size);
	return malloc(i_size);
	//return _aligned_malloc(i_size, 4);
}

void operator delete [](void* i_ptr)
{
	// replace with calls to your HeapManager or FixedSizeAllocators
	printf("delete [] 0x%" PRIXPTR "\n", reinterpret_cast<uintptr_t>(i_ptr));
	return free(i_ptr);
	//return _aligned_free(i_ptr);
}
