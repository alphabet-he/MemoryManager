#pragma once

struct FixedSizeAllocator
{
	int FixedSize;
	int BlockNum;
	//size_t baseAddr; // user memory starts from - after bit array
};
