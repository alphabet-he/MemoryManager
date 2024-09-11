#include "FixedSizeAllocator.h"
#include <Windows.h>
#include <cassert>
#include "WriteMemory.h"


void* FixedSizeAllocator::alloc()
{
    long ind = GetFirstFreeBlock();
    if (ind < 0) return nullptr;
    else {
        SetInUse(ind); // set the flag as cannot use
        // gap = bitArray + memory blocks before the free block
        size_t gap = BlockNum / 8 + ind * FixedSize;
        return AddPtrSize(BaseAddr, gap);
    }
}

bool FixedSizeAllocator::free(void* ptr)
{
    uintptr_t ptrValue = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t baseValue = reinterpret_cast<uintptr_t>(BaseAddr);

    unsigned long ind = (ptrValue - baseValue - BlockNum / 8) / FixedSize;
    if (IsBlockInUse(ind)) { // in use now
        SetFree(ind); // set the bit as 1, can use
        return true;
    }
    else {
        return false;
    }
    
}

bool FixedSizeAllocator::Contains(void* ptr) const
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
    SetAllFree();
}

void FixedSizeAllocator::SetAllInUse()
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        *checkingAddr = 0;
        checkingAddr++;
    }
}

void FixedSizeAllocator::SetAllFree()
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        *checkingAddr = (char)0xFF;
        checkingAddr++;
    }
}

bool FixedSizeAllocator::AreAllUsed() const
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        if (*checkingAddr != 0) return false;
        checkingAddr++;
    }
    return true;
}

bool FixedSizeAllocator::AreAllFree() const
{
    int byteNum = BlockNum / 8;
    char* checkingAddr = (char*)BaseAddr;

    for (int i = 0; i < byteNum; i++) {
        if (*checkingAddr != 0xFF) return false;
        checkingAddr++;
    }
    return true;
}

bool FixedSizeAllocator::IsBlockInUse(unsigned long ind) const
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

// can use
void FixedSizeAllocator::SetFree(unsigned long ind)
{
    assert(ind >= 0);
    // Calculate the byte offset and bit position within the byte
    size_t byteOffset = ind / 8;
    size_t bitPosInByte = ind % 8;

    char* base = (char*)BaseAddr;
    // Set the specified bit to 1
    base[byteOffset] |= (1 << (7 - bitPosInByte));
}

// cannot use
void FixedSizeAllocator::SetInUse(unsigned long ind)
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

//bool FixedSizeAllocator::GetFirstClearBit() const
//{
//    return false;
//}

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

bool FixedSizeAllocator::operator[](size_t i_index) const
{
    return false;
}
