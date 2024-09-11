# Memory Manager
## Junxuan Hu 

The project is a simple memory manager system using both fixed sized memory manager and dynamic heap manager. 

This document will explain some unique feature in the program.

### Memory System Composition with Easily-Editable Parameters

The memory system consists of two parts. One is fixed sized memory which takes 40% of total memory space, managed by several fixed size allocators. The other is free memory space that takes 60% of total memory, managed by the dynamic heap manager. If the required size in ```malloc(i_size)``` is within fixed size allocators' capacity, target FSA will allocate the memory in fixed sized space. Otherwise, the dynamic heap manager will allocate it in the free memory. For example, I have my fixed sizes being

```
const static size_t FixedSizes[] = { 16, 32, 64, 128, 196, 256 };
```

```malloc(96)``` will be allocated by FSAs in the fixed sized memory, while ```malloc(1024)``` will be allocated by the dynamic heap manager in the free memory space. 

```
// codes in Allocators.cpp

void* __cdecl malloc(size_t i_size){

    // determine whether it belongs to FSA
    int FSACount = sizeof(FixedSizes) / sizeof(FixedSizes[0]);

    // if it larger than the largest size
    if (i_size > FixedSizes[FSACount - 1]) {
        // dynamic heap manager will allocate it
        return HeapManagerProxy::alloc(p_HeapManager, i_size);
    }

    // the size can be allocated by FSAs
    else {
        ...
        target_FSA.alloc();
    }
}
```

If one fixed size allocator has its memory space full, but asked to allocate another block, it will try expanding its memory space twice as large. If there is no enough space to expand it twice as large, it tries to expand it 1.5 times as large. If the expansion still fails, the dynamic heap manager will allocate the memory.

```
// codes in MemorySystem.cpp

FixedSizeAllocator* ExpandFixedSizeAllocator(unsigned long fixedSize)
{
    ...
	// ideal size and block nums
	unsigned long blockNum = FSAsize / fixedSize;

	FixedSizeAllocator* FSA = HeapManagerProxy::CreateFixedSizeAllocator(p_HeapManager, fixedSize, blockNum);

	// if there is enough space to expand it twice as large, return true
	if (FSA) return FSA;

	// if there is no enough space to have another as large FSA,
	// try enlarge it to 1.5 times as large
	blockNum /= 2;
	return HeapManagerProxy::CreateFixedSizeAllocator(p_HeapManager, fixedSize, blockNum);
}
```
```
// codes in Allocators.cpp

void* __cdecl malloc(size_t i_size){
    ...
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
```

I refenrenced the form *Allocation Count by Size* from the slides of the last class to determine the ratio of each FSA's memory space.

| Memory Size      | Count | Total | Ratio
| ----------- | ----------- |------|-----|
| 8  | 62661 | 501288 | 0.0085
| 16 | 25192 | 403072 | 0.0068
| 32  | 116225 | 3719200 | 0.063
| 64 | 253504 | 16224256 | 0.275
| 128  | 137495 | 17599360 | 0.298
| ... | 

I stored these numerics as static parameters in ```MemorySystem.h```, so that they should be easy to modify.

```
const static float FixedSizedRatio = 0.4f;
const static size_t FixedSizes[] = { 16, 32, 64, 128, 196, 256 };
const static float SizeRatios[] = {0.0085f, 0.0068f, 0.063f, 0.275f, 0.298f, 0.190f, 0.157f};
```

```
// codes in MemorySystem.cpp

bool InitializeMemorySystem(void * i_pHeapMemory, size_t i_sizeHeapMemory, unsigned int i_OptionalNumDescriptors)
{
	...
	
	// create fixed size allocators by memory system params
	int FSACount = sizeof(FixedSizes) / sizeof(FixedSizes[0]);

	for (int i = 0; i < FSACount; i++) {

		// ideal size and block nums
		size_t FSAsize = i_sizeHeapMemory * FixedSizedRatio * SizeRatios[i];
		unsigned long blockNum = FSAsize / FixedSizes[i];
	
		// alocate the fixed size allocator by dynamic heap manager
		FixedSizeAllocator* FSA = HeapManagerProxy::CreateFixedSizeAllocator(DynamicHeapManager, FixedSizes[i], blockNum);
		
		// no enough space to allocate
		if (!FSA) return false;
	}

	return true;
}
```


### Write Memory

This program directly wirtes data to memory instead of using higher-level codes such as ```new``` or ```malloc```. 

```
// codes in WriteMemory.cpp

void writeDWord(size_t baseAddr, unsigned long data)//32b-4B
{
	unsigned long* nb = (unsigned long*)baseAddr;
	*nb = data;
}

void writeQWord(size_t baseAddr, unsigned long long data)//64b-8B
{
	unsigned long long* nb = (unsigned long long*)baseAddr;
	*nb = data;
}
```

I uses ```sizeof(size_t)``` to distinguish x86 and x64 OS. 
```
// codes in WriteMemory.cpp

void writeSizeT(size_t baseAddr, size_t data) {
	if (sizeof(size_t) == 4) writeDWord(baseAddr, (unsigned long)data); // WIN32
	else if (sizeof(size_t) == 8) writeQWord(baseAddr, (unsigned long long)data); //WIN64
}
```

An example of how to write a custom struct/class.

```
// codes in HeapManager.h

struct HeapManager
{
	void* pHeapMemory; 
	size_t sizeHeap; 
	MemoryBlockHeader* FreeList;
	unsigned long numDescriptors;
};
```
```
// codes in WriteMemory.cpp

void writeHM(void* writingAddr, size_t _pHeapMemory,
	size_t _sizeHeap, unsigned long _numDescriptors, size_t FreeListAddr) {
	writeSizeT((size_t)writingAddr, _pHeapMemory);
	writeSizeT((size_t)writingAddr + sizeof(size_t), _sizeHeap);
	writeSizeT((size_t)writingAddr + sizeof(size_t) * 2, FreeListAddr);
	writeDWord((size_t)writingAddr + sizeof(size_t) * 3, _numDescriptors);
}
```

### BitArray

I did not create a ```BitArray``` class, but I still used bits to record the usage of memory blocks in fixed size allocators. In memory, the bits of 'BitArray' starts after parameters of FixedSizeAllocator class, and ends before user memory blocks. 

I used bitwise operators and ```_BitScanReverse()``` to operate on the bits. 

```
// codes in FixedSizeAllocator.cpp

 long FixedSizeAllocator::GetFirstFreeBlock() const
{
    // during the creation, assertion guaranteed that blockNum % 8 == 0;
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;
    for (int i = 0; i < byteNum; i++) {
        // there exist a set bit
        if (*checkingAddr != 0) {
            unsigned long index; // This variable will store the position of the first set bit
            unsigned long maskedValue = (unsigned long)(*checkingAddr) & 0x000000FF; // Mask the byte
            if (_BitScanReverse(&index, maskedValue)) {
                int leftInd = 8 - static_cast<int>(index); // the position of a set bit from left
                return i * 8 + leftInd - 1;
            }
            else
            {
                assert(false);
            }
        }
        // there is no set bit, go to next 8 bit
        checkingAddr++;
    }
    return -1;
}
```

*I wanted to scan 8 bits - 1 byte once a time, because I think it won't be efficient if I scan more bits, such as 32b once a time. However,  ```_BitScanReverse()``` seems to always interprate its argument as ```int32``` insted of a byte. I had to mask my byte and cast it into ```int32```, since the assignment asks to use ```_BitScanReverse()```.*