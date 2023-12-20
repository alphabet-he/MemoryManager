#include "HeapManagerProxy.h"
#include "HeapManager.h"
#include "MemoryBlockHeader.h"
#include <cassert>
#include "WriteMemory.h"


HeapManager* HeapManagerProxy::CreateHeapManager(void* pHeapMemory, size_t sizeHeap, unsigned int numDescriptors)
{
	// heap starts after its info: pHeapMemory, sizeHeap, numDescriptors, FreeList
	void* heapBaseAddr = AddPtrSize(pHeapMemory, _HeapManagerSize);
	
	// user memory base addr: starts after the block header
	void* baseAddr = AddPtrSize(heapBaseAddr, _BlockHeaderSize);

	// write free memory list
	writeMBH(heapBaseAddr, (size_t)baseAddr, sizeHeap - _BlockHeaderSize - _HeapManagerSize, (size_t)nullptr, (size_t)nullptr, (size_t)nullptr, (size_t)nullptr, false, false);
	MemoryBlockHeader* FreeList = (MemoryBlockHeader*)heapBaseAddr;

	// write Heap Manager
	writeHM(pHeapMemory, (size_t)heapBaseAddr, sizeHeap - _HeapManagerSize, (long)numDescriptors, (long long)FreeList);

	HeapManager* heapManager = (HeapManager*)pHeapMemory;
	size_t a = sizeof(HeapManager);
	return heapManager;
}

void* HeapManagerProxy::alloc(HeapManager* pHeapManager, size_t sizeAlloc)
{
	return alloc(pHeapManager, sizeAlloc, 0);
	
}

void* HeapManagerProxy::alloc(HeapManager* pHeapManager, size_t sizeAlloc, unsigned int alignment)
{
	size_t requiredSize = sizeAlloc + _BlockHeaderSize; // size to allocate + memory block header size

	// it's not even possible to put in such a huge thing
	if (requiredSize > pHeapManager->sizeHeap) {
		return nullptr;
	}

	MemoryBlockHeader* pFreeBlock = pHeapManager->FreeList;
	unsigned int alignUp;

	while (pFreeBlock) {

		if (alignment != 0) { // there is alignment requirement
			alignUp = alignment - (long long)pFreeBlock->pBaseAddr % alignment;
		}
		else { // there is no alignment requirement
			alignUp = 0;
		}
		
		if (pFreeBlock->BlockSize >= (sizeAlloc + alignUp) && !pFreeBlock->used) {
			// the free block is enough space to allocate the user size with alignment, 
			// but not sure whether there is enough space to allocate another header (for the next memory block)
			break;
		}
		pFreeBlock = pFreeBlock->pNextBlock;
	}
	// there is no free block big enough
	if (!pFreeBlock) {
		return NULL;
	}

	//if ((long)pFreeBlock == (long)pHeapManager + 0x1d89) {
	//	register int a = 0;
	//}

	pFreeBlock->used = true;

	// if there is no room for another memory block
	if (pFreeBlock->BlockSize <= (requiredSize + alignUp)) {

		// connect freelist
		
		// prevFree.nextFree = nextFree
		if (pFreeBlock->pPrevFreeBlock == nullptr) {
			pHeapManager->FreeList = pFreeBlock->pNextFreeBlock;
		}
		else {
			pFreeBlock->pPrevFreeBlock->pNextFreeBlock = pFreeBlock->pNextFreeBlock;
		}
		// nextFree.prevFree = prevFree
		if (pFreeBlock->pNextFreeBlock) {
			pFreeBlock->pNextFreeBlock->pPrevFreeBlock = pFreeBlock->pPrevFreeBlock;
		}

		// prev/next blocks are already connected, no need to modify
	}

	// if there is room for another memory block
	else {
		// create another header at the end of user memory
		void* newHeaderAddr = AddPtrSize(pFreeBlock->pBaseAddr, sizeAlloc + alignUp);
		void* newBlockBaseAddr = AddPtrSize(newHeaderAddr, _BlockHeaderSize);
		size_t newBlockSize = pFreeBlock->BlockSize - requiredSize - alignUp; // shrink size
		writeMBH(newHeaderAddr,
			(size_t)newBlockBaseAddr, newBlockSize,
			(size_t)pFreeBlock->pNextBlock, (size_t)MinusPtrSize(pFreeBlock->pBaseAddr, _BlockHeaderSize),
			(size_t)pFreeBlock->pNextFreeBlock, (size_t)pFreeBlock->pPrevFreeBlock,
			false, false);

		MemoryBlockHeader* newNextHeader = (MemoryBlockHeader*)newHeaderAddr;

		// connect new memory block to free list

		// connect new block to previous free block
		if (pFreeBlock->pPrevFreeBlock == nullptr) {
			pHeapManager->FreeList = newNextHeader;
		}
		else {
			pFreeBlock->pPrevFreeBlock->pNextFreeBlock = newNextHeader;
		}
		// connect new block to next free block
		if (pFreeBlock->pNextFreeBlock) {
			pFreeBlock->pNextFreeBlock->pPrevFreeBlock = newNextHeader;
		}

		// connect new block to prev/next block
		if (pFreeBlock->pNextBlock) {
			pFreeBlock->pNextBlock->pPrevBlock = newNextHeader;
		}
		pFreeBlock->pNextBlock = newNextHeader;

		// modify block size
		pFreeBlock->BlockSize = sizeAlloc; 
		
	}


	// if the user memory address needs align up
	if (alignUp != 0) { 
		// modify the user address
		void* alignedBaseAddr = AddPtrSize(pFreeBlock->pBaseAddr, alignUp);
		//// modify block size
		//size_t alignedSize = pFreeBlock->BlockSize + alignUp;
		// expand the previous block
		if (pFreeBlock->pPrevBlock) {
			pFreeBlock->pPrevBlock->BlockSize += alignUp;
		}
		// move the header to the front of the user memory
		void* alignedHeaderAddr = MinusPtrSize(alignedBaseAddr, _BlockHeaderSize);
		// modify the next block's prev pointer
		if (pFreeBlock->pNextBlock) {
			pFreeBlock->pNextBlock->pPrevBlock = (MemoryBlockHeader*)alignedHeaderAddr;
		}
		// modify the prev block's next pointer
		if (pFreeBlock->pPrevBlock) {
			pFreeBlock->pPrevBlock->pNextBlock = (MemoryBlockHeader*)alignedHeaderAddr;
		}
		// snapshot for pFreeBlock
		size_t newSize = pFreeBlock->BlockSize;
		void* newNextBlock = pFreeBlock->pNextBlock;
		void* newPrevBlock = pFreeBlock->pPrevBlock;
		void* newNextFreeBlock = pFreeBlock->pNextFreeBlock;
		void* newPrevFreeBlock = pFreeBlock->pPrevFreeBlock;
		// write in a new header in the aligned address
		writeMBH(alignedHeaderAddr, (size_t)alignedBaseAddr, newSize,
			(size_t)newNextBlock, (size_t)newPrevBlock,
			(size_t)newNextFreeBlock, (size_t)newPrevFreeBlock, true, false);
		//if (alignedBaseAddr == (void*)0x012b1c80) {
		//	int a;
		//}
		MemoryBlockHeader* alignedHeader = (MemoryBlockHeader*)MinusPtrSize(alignedBaseAddr, _BlockHeaderSize);
		//if (alignedHeader->pNextBlock) {
		//	printf("Next block size: %zu\n", alignedHeader->pNextBlock->BlockSize);
		//}
		//if (alignedHeader->pPrevBlock) {
		//	printf("Prev block size: %zu\n", alignedHeader->pPrevBlock->BlockSize);
		//}
		//printf("Current block size: %zu\n", alignedHeader->BlockSize);

		//if (alignedHeader->pPrevBlock && alignedHeader->pNextBlock) {
		//	if (alignedHeader->pPrevBlock->BlockSize +
		//		alignedHeader->pNextBlock->BlockSize >
		//		pHeapManager->sizeHeap) {
		//		register int a = 0;
		//	}
		//}

		//if ((long)alignedBaseAddr == (long)pHeapManager + 0x1d6c) {
		//	register int a = 0;
		//}
		return alignedBaseAddr;
	}

	else {
		//if (pFreeBlock->pBaseAddr == (void*)0x012b1c80) {
		//	int a;
		//}
		//printf("%zu\n", pFreeBlock->BlockSize);
		return pFreeBlock->pBaseAddr;
	}
	
}




void HeapManagerProxy::Collect(HeapManager* pHeapManager)
{
	// I do collect every time I free:
	// in my understanding, it's connecting the prev and next free block
}

bool HeapManagerProxy::Contains(HeapManager* pHeapManager, void* pPtr)
{
	void* heapMemStart = pHeapManager->pHeapMemory;
	void* heapMemEnd = AddPtrSize(heapMemStart, pHeapManager->sizeHeap);
	if ((long long)heapMemStart <= (long long)pPtr && (long long)pPtr <= (long long)heapMemEnd) { return true; }
	return false;
}

bool HeapManagerProxy::IsAllocated(HeapManager* pHeapManager, void* pPtr)
{
	MemoryBlockHeader* header = (MemoryBlockHeader*) MinusPtrSize(pPtr, _BlockHeaderSize);
	//printf("%zu\n", header->BlockSize);
	//if (header->pNextBlock) {
	//	printf("Next block size: %zu\n", header->pNextBlock->BlockSize);
	//}
	//if (header->pPrevBlock) {
	//	printf("Prev block size: %zu\n", header->pPrevBlock->BlockSize);
	//}
	return header->used;
}

bool HeapManagerProxy::free(HeapManager* pHeapManager, void* pPtr)
{
	MemoryBlockHeader* header = (MemoryBlockHeader*)MinusPtrSize(pPtr, _BlockHeaderSize);
	// if the place is not used, return false
	if (!header->used) { return false; }
	else
	{
		// if there is a next block and it is free
		if (header->pNextBlock && !header->pNextBlock->used) {
			header->used = false;
			// expand block size: original + next block header + next block size
			header->BlockSize += header->pNextBlock->BlockSize;
			header->BlockSize += _BlockHeaderSize;
			// connect in the free list
			header->pPrevFreeBlock = header->pNextBlock->pPrevFreeBlock;
			header->pNextFreeBlock = header->pNextBlock->pNextFreeBlock;

			if (header->pPrevFreeBlock == nullptr) {
				pHeapManager->FreeList = header;
			}

			// connect to next block
			if (header->pNextBlock->pNextBlock) {
				header->pNextBlock->pNextBlock->pPrevBlock = header;
			}
			header->pNextBlock = header->pNextBlock->pNextBlock;
			//// remove the next block from linked list
			//header->pNextBlock->pPrevBlock = nullptr;
			//header->pNextBlock->pNextBlock = nullptr;
			//header->pNextBlock->pPrevFreeBlock = nullptr;
			//header->pNextBlock->pPrevFreeBlock = nullptr;
		}

		// if there is a previous block and it is free
		if (header->pPrevBlock && !header->pPrevBlock->used) {
			header->used = false;
			// expand previous block size
			header->pPrevBlock->BlockSize += header->BlockSize;
			header->pPrevBlock->BlockSize += _BlockHeaderSize;
			// connect the previous block to the next block
			header->pPrevBlock->pNextBlock = header->pNextBlock;
			if (header->pNextBlock) {
				header->pNextBlock->pPrevBlock = header->pPrevBlock;
			}
			// no need to modify the free list
			//// remove the block from the linked list
			//header->pPrevBlock = nullptr;
			//header->pNextBlock = nullptr;
		}

		// whether at least one of the two if-s above is executed?
		// if yes, return
		if (!header->used) return true;

		// else, none of the two if-s above is executed.
		// in other words, there is no free block on previous or next
		// there are four possible situations:
		// a) previous is null and next is used
		// b) next is null and previous is used
		// c) there are previous and next blocks, but both are used
		// d) no previous nor next blocks
		else {
			// traverse forward to find the nearest free block
			MemoryBlockHeader* pNearestFreePrev = header->pPrevBlock;
			while (pNearestFreePrev)
			{
				if (!pNearestFreePrev->used) break;
				pNearestFreePrev = pNearestFreePrev->pPrevBlock;
			}
			// if there is a free block previous to the current block
			if (pNearestFreePrev) {
				// connect to free list
				header->pPrevFreeBlock = pNearestFreePrev;
				header->pNextFreeBlock = pNearestFreePrev->pNextFreeBlock;
				if (pNearestFreePrev->pNextFreeBlock) {
					pNearestFreePrev->pNextFreeBlock->pPrevFreeBlock = header;
				}
				pNearestFreePrev->pNextFreeBlock = header;
				header->used = false;
				return true;
			}

			// if there is no free block previous to the current block
			else
			{
				if (pHeapManager->FreeList) { // there exist a free block, but it is after the current block
					// the first block in the free list becomes the next free block
					header->pNextFreeBlock = pHeapManager->FreeList;
					// connect with the next free block
					header->pNextFreeBlock->pPrevFreeBlock = header;
					// the current block becomes the head of free list
					pHeapManager->FreeList = header;
					header->used = false;
					return true;
				}
				else { // there is no free block at all, the current block is the only free blcok
					pHeapManager->FreeList = header;
					header->used = false;
					return true;
				}
				
			}
		}
		
	}
	return false;
}

void HeapManagerProxy::Destroy(HeapManager* pHeapManager)
{
	// mark the whole heap as an unused memory block
	MemoryBlockHeader* beginBlock = (MemoryBlockHeader*)(pHeapManager->pHeapMemory);
	beginBlock->BlockSize = pHeapManager->sizeHeap;
	beginBlock->pNextBlock = nullptr;
	beginBlock->pNextFreeBlock = nullptr;
	beginBlock->used = false;
	beginBlock->fixedSized = false;
}

void HeapManagerProxy::ShowFreeBlocks(HeapManager* pHeapManager)
{
}

void HeapManagerProxy::ShowOutstandingAllocations(HeapManager* pHeapManager)
{
}

FixedSizeAllocator* HeapManagerProxy::CreateFixedSizeAllocator(HeapManager* pHeapManager, unsigned int fixedSize, unsigned int blockNum)
{
	assert(blockNum % 8 == 0);
	
	// Size of FSA = FSA header + bitArray + user memory space
	size_t requiredSize = sizeof(FixedSizeAllocator) + blockNum / 8 + blockNum * fixedSize;
	void* writingAddr = HeapManagerProxy::alloc(pHeapManager, (size_t)requiredSize);
	
	// mark it as fixed sized
	MemoryBlockHeader* MBH = (MemoryBlockHeader*)MinusPtrSize(writingAddr, sizeof(MemoryBlockHeader));
	MBH->fixedSized = true;

	// if there is enough space to create an FSA
	if (writingAddr) {
		void* baseAddr = AddPtrSize(writingAddr, sizeof(FixedSizeAllocator));
		writeFSA(writingAddr, baseAddr, fixedSize, blockNum);
		// Set FSA as all free
		FixedSizeAllocator* FSA = (FixedSizeAllocator*)writingAddr;
		FSA->SetAllFree();
		return FSA;
	}
	else
	{
		return nullptr;
	}
	
}


