#include "FixedSizeAllocator.h"
#include <Windows.h>
#include <cassert>
#include "WriteMemory.h"


void* FixedSizeAllocator::alloc(size_t sizeAlloc)
{
    return nullptr;
}

bool FixedSizeAllocator::free(void* ptr)
{
    return false;
}

inline bool FixedSizeAllocator::Contains(void* ptr) const
{
    // the leftmost address of user memory space (after bitArray)
    void* minAddr = AddPtrSize(BaseAddr, BlockNum / 8);
    // the rightmost address of user memory space (after bitArray)
    void* maxAddr = AddPtrSize(minAddr, BlockNum * FixedSize);

    uintptr_t ptrValue = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t leftValue = reinterpret_cast<uintptr_t>(minAddr);
    uintptr_t rightValue = reinterpret_cast<uintptr_t>(maxAddr);

    if (ptrValue < leftValue || ptrValue > rightValue) return false;
    else {
        // whether this is an aligned addr
        if ((ptrValue - leftValue) % FixedSize == 0) return true;
        else assert(false);
    }
    return false;
}

FixedSizeAllocator::~FixedSizeAllocator()
{
}

void FixedSizeAllocator::ClearAll()
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        *checkingAddr = 0;
    }
}

void FixedSizeAllocator::SetAll()
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        *checkingAddr = 0xFF;
    }
}

inline bool FixedSizeAllocator::AreAllBitsClear() const
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        if (*checkingAddr != 0) return false;
    }
    return true;
}

inline bool FixedSizeAllocator::AreAllBitsSet() const
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        if (*checkingAddr != 0xFF) return false;
    }
    return true;
}

inline bool FixedSizeAllocator::IsBitClear(unsigned long ind) const
{
    assert(ind >= 0);
    // Calculate the byte offset and bit position within the byte
    size_t byteOffset = ind / 8;
    size_t bitPosInByte = ind % 8;

    // a mask that only the bit in question is 1, others are 0
    unsigned char mask = 1 << (7 - bitPosInByte);

    // get the bit in question
    char* base = (char*)BaseAddr;
    unsigned char masked = base[byteOffset] &= mask;

    return(masked == 0);
}

inline void FixedSizeAllocator::SetBit(unsigned long ind)
{
    assert(ind >= 0);
    // Calculate the byte offset and bit position within the byte
    size_t byteOffset = ind / 8;
    size_t bitPosInByte = ind % 8;

    char* base = (char*)BaseAddr;
    // Set the specified bit to 1
    base[byteOffset] |= (1 << (7 - bitPosInByte));
}

inline void FixedSizeAllocator::ClearBit(unsigned long ind)
{
    assert(ind >= 0);
    // Calculate the byte offset and bit position within the byte
    size_t byteOffset = ind / 8;
    size_t bitPosInByte = ind % 8;

    char* base = (char*)BaseAddr;

    // Create a mask with all bits set to 1 except the specified bit
    unsigned char mask = ~(1 << (7 - bitPosInByte));

    // Clear the specified bit to 0
    base[byteOffset] &= mask;
}

bool FixedSizeAllocator::GetFirstClearBit() const
{
    return false;
}

 long FixedSizeAllocator::GetFirstSetBit() const
{
    // during the creation, assertion guaranteed that blockNum % 8 == 0;
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;
    for (int i = 0; i < byteNum; i++) {
        // there exist a set bit
        if (*checkingAddr != 0) {
            unsigned long index; // This variable will store the position of the first set bit
            if (_BitScanReverse(&index, *checkingAddr)) {
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

bool FixedSizeAllocator::operator[](size_t i_index) const
{
    return false;
}
