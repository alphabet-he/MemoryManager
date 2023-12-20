#include <Windows.h>

#include "MemorySystem.h"
#include "HeapManagerUnitTest.h"
#include "FixedSizeAllocator.h"

#include <assert.h>
#include <algorithm>
#include <vector>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG
#include <iostream>
#include <bitset>

bool MemorySystem_UnitTest();

int FindFirstSetBitPosition(char value) {
	unsigned long index; // This variable will store the position of the first set bit

	// Use _BitScanForward to find the position of the first set bit
	if (_BitScanReverse(&index, value)) {
		return static_cast<int>(index); // Return the position if a set bit is found
	}
	else {
		return -1; // Return -1 if no set bits are found
	}
}

void SetFifthBit(unsigned char* baseAddress, unsigned long ind) {
	// Assuming baseAddress points to a valid memory location

	// Calculate the byte offset and bit position within the byte
	size_t byteOffset = ind / 8;
	size_t bitPosInByte = ind % 8;

	// Set the specified bit to 1
	baseAddress[byteOffset] |= (1 << (7 - bitPosInByte));
}



int main(int i_arg, char **)
{
	/*
	// Example: Allocate memory for two bytes
	unsigned char* memory = new unsigned char[4];

	for (size_t i = 0; i < 4; ++i) {
		memory[i] = 0;
	}

	// Call the function to set the fifth bit to 1
	SetFifthBit(memory, 17);

	// Print the result
	std::cout << "Memory content after setting the 263rd bit (MSB): ";
	for (size_t i = 0; i < 4; ++i) {
		std::cout << std::bitset<8>(memory[i]) << " ";
	}
	std::cout << std::endl;
	// Don't forget to free the allocated memory
	delete[] memory;

	
	char myValue = '9'; // Example value with set bits at positions 3 and 5

	int i;
	for (i = 0; i < 8; i++) {
		printf("%d", !!((myValue << i) & 0x80));
	}
	printf("\n");

	int firstSetBitPosition = FindFirstSetBitPosition(myValue);

	if (firstSetBitPosition != -1) {
		std::cout << "The first set bit is at position: " << firstSetBitPosition << std::endl;
	}
	else {
		std::cout << "No set bits found in the value." << std::endl;
	}

	bool heapPassed = HeapManager_UnitTest();
	if (heapPassed)
		printf("heap passed\n");
	else
		printf("heap FAILED\n");

	*/

	

	const size_t 		sizeHeap = 1024 * 1024;

	// you may not need this if you don't use a descriptor pool
	const unsigned int 	numDescriptors = 2048;

	// Allocate memory for my test heap.
	void * pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	// Create your HeapManager and FixedSizeAllocators.
	InitializeMemorySystem(pHeapMemory, sizeHeap, numDescriptors);

	void* a = malloc(sizeof(int));
	free(a);

	bool success = MemorySystem_UnitTest();
	if (success)
		printf("test passed\n");
	else
		printf("test FAILED\n");
	assert(success);

	// Clean up your Memory System (HeapManager and FixedSizeAllocators)
	DestroyMemorySystem();

	HeapFree(GetProcessHeap(), 0, pHeapMemory);

	// in a Debug build make sure we didn't leak any memory.
#if defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif // _DEBUG

	return 0;
}

bool MemorySystem_UnitTest()
{
	const size_t maxAllocations = 10 * 1024;
	std::vector<void *> AllocatedAddresses;

	long	numAllocs = 0;
	long	numFrees = 0;
	long	numCollects = 0;

	size_t totalAllocated = 0;

	// reserve space in AllocatedAddresses for the maximum number of allocation attempts
	// prevents new returning null when std::vector expands the underlying array
	AllocatedAddresses.reserve(10 * 1024);

	// allocate memory of random sizes up to 1024 bytes from the heap manager
	// until it runs out of memory
	do
	{
		const size_t		maxTestAllocationSize = 1024;

		size_t			sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));

		void * pPtr = malloc(sizeAlloc);

		// if allocation failed see if garbage collecting will create a large enough block
		if (pPtr == nullptr)
		{
			Collect();

			pPtr = malloc(sizeAlloc);

			// if not we're done. go on to cleanup phase of test
			if (pPtr == nullptr)
				break;
		}

		AllocatedAddresses.push_back(pPtr);
		numAllocs++;

		totalAllocated += sizeAlloc;

		// randomly free and/or garbage collect during allocation phase
		const unsigned int freeAboutEvery = 0x07;
		const unsigned int garbageCollectAboutEvery = 0x07;

		if (!AllocatedAddresses.empty() && ((rand() % freeAboutEvery) == 0))
		{
			void * pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			free(pPtrToFree);
			numFrees++;
		}
		else if ((rand() % garbageCollectAboutEvery) == 0)
		{
			Collect();

			numCollects++;
		}

	} while (numAllocs < maxAllocations);

	// now free those blocks in a random order
	if (!AllocatedAddresses.empty())
	{
		// randomize the addresses
		std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

		// return them back to the heap manager
		while (!AllocatedAddresses.empty())
		{
			void * pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			delete[] pPtrToFree;
		}

		// do garbage collection
		Collect();
		// our heap should be one single block, all the memory it started with

		// do a large test allocation to see if garbage collection worked
		void * pPtr = malloc(totalAllocated / 2);

		if (pPtr)
		{
			free(pPtr);
		}
		else
		{
			// something failed
			return false;
		}
	}
	else
	{
		return false;
	}

	// this new [] / delete [] pair should run through your allocator
	char * pNewTest = new char[1024];
	
	delete[] pNewTest;

	// we succeeded
	return true;
	
}
