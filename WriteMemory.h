#pragma once
#include <cstdint>

const int _byteSize = sizeof(bool);
const int _longlongSize = sizeof(long long);
const int _longSize = sizeof(long);


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

void writeFSA(void* writingAddr, void* baseAddr, unsigned int fixedSize, unsigned int blockNum);

/*

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

*/
inline void* AddPtrSize(const void* ptr, const size_t byteSize) {
	return (void*)((intptr_t)ptr + byteSize);
};

inline void* MinusPtrSize(const void* ptr, const size_t
	byteSize) {
	return (void*)((intptr_t)ptr - byteSize);
};
