#pragma once

struct MemoryBlockHeader
{
	void* pBaseAddr;
	size_t BlockSize;
	struct MemoryBlockHeader* pNextBlock;
	struct MemoryBlockHeader* pPrevBlock;
	struct MemoryBlockHeader* pNextFreeBlock;
	struct MemoryBlockHeader* pPrevFreeBlock;
	bool used;
	bool fixedSized; // is fixed size allocator or not
};

const size_t _BlockHeaderSize = sizeof(MemoryBlockHeader);
