#pragma once
#include <cstdint>

void writeByte(const size_t baseAddr, const unsigned char data);//8b-1B

void writeWord(const size_t baseAddr, const unsigned short data);//16b-2B

void writeDWord(const size_t baseAddr, const unsigned long data);//32b-4B

void writeQWord(const size_t baseAddr, const unsigned long long data);//64b-8B

void writeMBH(void* writingAddr,
	size_t baseAddr, size_t blockSize,
	size_t nextAddr, size_t prevAddr,
	size_t nextFreeAddr, size_t prevFreeAddr,
	bool usedFlag, bool fixedSizedFlag);

void writeHM(void* writingAddr, size_t _pHeapMemory,
	size_t _sizeHeap, unsigned long _numDescriptors, size_t FreeListAddr);

inline void* AddPtrSize(const void* ptr, const size_t byteSize) {
	return (void*)((intptr_t)ptr + byteSize);
};

inline void* MinusPtrSize(const void* ptr, const size_t
	byteSize) {
	return (void*)((intptr_t)ptr - byteSize);
};
