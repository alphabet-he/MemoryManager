#pragma once
class FixedSizeAllocator
{
public:

	void* alloc();

	bool free(void* ptr);

	bool Contains(void* ptr) const;

	~FixedSizeAllocator();

	void SetAllInUse();
	void SetAllFree();

	// are all blocks used?
	bool AreAllUsed() const;
	// are all blocks vacant?
	bool AreAllFree() const;

	// 0: cannot use; 1: can use
	bool IsBlockInUse(unsigned long ind) const; 

	void SetFree(unsigned long ind);
	void SetInUse(unsigned long ind);

	//bool GetFirstClearBit() const;
	long GetFirstFreeBlock() const;

	bool operator[](size_t i_index) const;

	inline size_t GetFixedSize() const { return FixedSize; };


private:

	FixedSizeAllocator() { BaseAddr = nullptr; FixedSize = 0; BlockNum = 0; };

protected:
	void* BaseAddr;
	unsigned int FixedSize;
	unsigned int BlockNum;
};

static unsigned long BitsInByte = 8;

