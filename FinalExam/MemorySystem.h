#pragma once
#include "HeapManagerProxy.h"
#include "HeapManager.h"

// params of the memory system
extern HeapManager* p_HeapManager;
const static float FixedSizedRatio = 0.4f;
const static size_t FixedSizes[] = { 16, 32, 64, 128, 196, 256 };
const static float SizeRatios[] = {0.0085f, 0.0068f, 0.063f, 0.275f, 0.298f, 0.190f, 0.157f};

// InitializeMemorySystem - initialize your memory system including your HeapManager and some FixedSizeAllocators
bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors);

// Collect - coalesce free blocks in attempt to create larger blocks
void Collect();

// DestroyMemorySystem - destroy your memory systems
void DestroyMemorySystem();

// ExpandFixedSizeAllocator - expand original fixed memory allocator space twice as large
FixedSizeAllocator* ExpandFixedSizeAllocator(unsigned long fixedSize);
