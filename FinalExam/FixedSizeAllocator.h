#pragma once
class FixedSizeAllocator
{
public:

	void* alloc(size_t sizeAlloc);

	bool free(void* ptr);

	inline bool Contains(void* ptr) const;

	~FixedSizeAllocator();

	void ClearAll();
	void SetAll();

	inline bool AreAllBitsClear() const;
	inline bool AreAllBitsSet() const;

	inline bool IsBitClear(unsigned long ind) const;

	inline void SetBit(unsigned long ind);
	inline void ClearBit(unsigned long ind);

	bool GetFirstClearBit() const;
	long GetFirstSetBit() const;

	bool operator[](size_t i_index) const;


private:

	FixedSizeAllocator() { BaseAddr = nullptr; FixedSize = 0; BlockNum = 0; };

protected:
	void* BaseAddr;
	unsigned int FixedSize;
	unsigned int BlockNum;
};

static unsigned long BitsInByte = 8;

