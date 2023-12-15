#pragma once
#include <cstdint>

void writeByte(const long long baseAddr, const unsigned char data);//8b-1B

void writeWord(const long long baseAddr, const unsigned short data);//16b-2B

void writeDWord(const long long baseAddr, const unsigned long data);//32b-4B

void writeQWord(const long long baseAddr, const unsigned long long data);//64b-8B

void writeMBH(void* writingAddr,
	unsigned long long baseAddr, unsigned long long blockSize,
	unsigned long long nextAddr, unsigned long long prevAddr,
	unsigned long long nextFreeAddr, unsigned long long prevFreeAddr,
	bool usedFlag);

void writeHM(void* writingAddr, unsigned long long _pHeapMemory,
	unsigned long long _sizeHeap, unsigned long _numDescriptors, unsigned long long FreeListAddr);

inline void* AddPtrSize(const void* ptr, const size_t byteSize) {
	return (void*)((intptr_t)ptr + byteSize);
};

inline void* MinusPtrSize(const void* ptr, const size_t
	byteSize) {
	return (void*)((intptr_t)ptr - byteSize);
};
