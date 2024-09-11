#include "WriteMemory.h"

void writeByte(size_t baseAddr, unsigned char data)//8b-1B
{
	char* nb = (char*)baseAddr;
	*nb = data;
}

void writeWord(size_t baseAddr, unsigned short data)//16b-2B
{
	unsigned short* nb = (unsigned short*)baseAddr;
	*nb = data;
}

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

void writeSizeT(size_t baseAddr, size_t data) {
	if (sizeof(size_t) == 4) writeDWord(baseAddr, (unsigned long)data); // WIN32
	else if (sizeof(size_t) == 8) writeQWord(baseAddr, (unsigned long long)data); //WIN64
}

void writeMBH(void* writingAddr,
	size_t baseAddr, size_t blockSize,
	size_t nextAddr, size_t prevAddr,
	size_t nextFreeAddr, size_t prevFreeAddr,
	bool usedFlag, bool fixedSizedFlag)
{
	writeSizeT((size_t)writingAddr, baseAddr);
	writeSizeT((size_t)writingAddr + sizeof(size_t), blockSize);
	writeSizeT((size_t)writingAddr + sizeof(size_t) * 2, nextAddr);
	writeSizeT((size_t)writingAddr + sizeof(size_t) * 3, prevAddr);
	writeSizeT((size_t)writingAddr + sizeof(size_t) * 4, nextFreeAddr);
	writeSizeT((size_t)writingAddr + sizeof(size_t) * 5, prevFreeAddr);
	writeByte((size_t)writingAddr + sizeof(size_t) * 6, usedFlag);
	writeByte((size_t)writingAddr + sizeof(size_t) * 6 + _byteSize, fixedSizedFlag);
};


void writeHM(void* writingAddr, size_t _pHeapMemory,
	size_t _sizeHeap, unsigned long _numDescriptors, size_t FreeListAddr) {
	writeSizeT((size_t)writingAddr, _pHeapMemory);
	writeSizeT((size_t)writingAddr + sizeof(size_t), _sizeHeap);
	writeSizeT((size_t)writingAddr + sizeof(size_t) * 2, FreeListAddr);
	writeDWord((size_t)writingAddr + sizeof(size_t) * 3, _numDescriptors);
	
}


void writeFSA(void* writingAddr, void* baseAddr, unsigned int fixedSize, unsigned int blockNum)
{
	writeSizeT((size_t)writingAddr, (size_t)baseAddr);
	writeDWord((size_t)writingAddr + sizeof(size_t), fixedSize);
	writeDWord((size_t)writingAddr + sizeof(size_t) + _longSize, blockNum);
}
