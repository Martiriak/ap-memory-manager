// Alessandro Pegoraro, 2022

#include "SmallObjAllocator.h"

#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream>

using namespace APMemory;

void SmallObjAllocator::FixedAllocator::Chunk::Init(const std::size_t BlockSize, const unsigned char BlocksNumber)
{
	assert(Data == nullptr);
	Data = static_cast<unsigned char*>(malloc(BlockSize * BlocksNumber));

	std::cout << " >>>  Chunk Init!\n";

	Reset(BlockSize, BlocksNumber);
}

void SmallObjAllocator::FixedAllocator::Chunk::Reset(const std::size_t BlockSize, const unsigned char BlocksNumber)
{
	assert(Data != nullptr);

	FirstAvailableBlockIndex = 0;
	NumberOfAvailableBlocks = BlocksNumber;

	// Init single linked list.
	unsigned char* iBlock = Data;
	for (unsigned char i = 0; i < BlocksNumber; iBlock += BlockSize)
	{
		*iBlock = ++i;
	}
}

void SmallObjAllocator::FixedAllocator::Chunk::Shutdown()
{
	assert(Data != nullptr);
	free(Data);
	Data = nullptr;

	std::cout << " >>>  Chunk Freed!";
}

void* SmallObjAllocator::FixedAllocator::Chunk::Alloc(const std::size_t BlockSize)
{
	assert(Data != nullptr);

	unsigned char* const NewlyAllocatedBlock = Data + (FirstAvailableBlockIndex * BlockSize);

	FirstAvailableBlockIndex = *NewlyAllocatedBlock;
	--NumberOfAvailableBlocks;

	std::cout << "---------------> Allocated, " << static_cast<short>(NumberOfAvailableBlocks) << " blocks remaining.\n";

	return NewlyAllocatedBlock;
}

void SmallObjAllocator::FixedAllocator::Chunk::Dealloc(void* BlockToDealloc, const std::size_t BlockSize)
{
	assert(Data != nullptr);

	unsigned char* const NewlyDeallocatedBlock = static_cast<unsigned char*>(BlockToDealloc);

	*NewlyDeallocatedBlock = FirstAvailableBlockIndex;
	++NumberOfAvailableBlocks;
	FirstAvailableBlockIndex = static_cast<unsigned char>((NewlyDeallocatedBlock - Data) / BlockSize);

	std::cout << "---------------> Deallocated, " << static_cast<short>(NumberOfAvailableBlocks) << " blocks available.\n";
}
