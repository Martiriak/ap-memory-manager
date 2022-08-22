// Alessandro Pegoraro, 2022

#include "SmallObjAllocator.h"

#include <cstdlib>
#include <cmath>
#include <cassert>

using namespace APMemory;


SmallObjAllocator::SmallObjAllocator(const std::size_t ChunkSize, const std::size_t MaxObjectSize)
{
	assert(MaxObjectSize % 2 == 0);

	Allocators.reserve( (std::size_t) std::log2(MaxObjectSize));

	// Risk of overflow of iAllocatorSize, but trust the small size of MaxObjectSize.
	for (std::size_t iAllocatorSize = 1; iAllocatorSize <= MaxObjectSize; iAllocatorSize <<= 1)
	{
		FixedAllocator Allocator;

		std::size_t BlocksInChunk = ChunkSize / iAllocatorSize;

		if (BlocksInChunk > UCHAR_MAX) BlocksInChunk = UCHAR_MAX;
		else if (BlocksInChunk < static_cast<std::size_t>(MinimumNumberOfBlocksInChunks))
			BlocksInChunk = static_cast<std::size_t>(MinimumNumberOfBlocksInChunks);

		Allocator.Init(iAllocatorSize, static_cast<unsigned char>(BlocksInChunk));
		Allocators.push_back(Allocator);
	}
}

SmallObjAllocator::~SmallObjAllocator()
{
	for (FixedAllocator& Allocator : Allocators)
	{
		Allocator.Reset();
	}

	Allocators.clear();
}

void* SmallObjAllocator::Alloc(const std::size_t BytesToAlloc)
{
	if (BytesToAlloc == 0 || BytesToAlloc > Allocators.back().GetBlockSize()) return nullptr;

	if (LastAllocationAllocator == nullptr || BytesToAlloc > LastAllocationAllocator->GetBlockSize())
	{
		for (FixedAllocator& Allocator : Allocators)
		{
			if (Allocator.GetBlockSize() >= BytesToAlloc)
			{
				LastAllocationAllocator = &Allocator;
				LastDeallocationAllocator = &Allocator;
				break;
			}
		}
	}

	assert(LastAllocationAllocator != nullptr);
	assert(LastAllocationAllocator->GetBlockSize() >= BytesToAlloc);
	return LastAllocationAllocator->Alloc();
}

void SmallObjAllocator::Dealloc(void* MemoryToDealloc, const std::size_t SizeOfMemoryToDealloc)
{
	if (MemoryToDealloc == nullptr) return;

	if (LastDeallocationAllocator == nullptr || SizeOfMemoryToDealloc > LastDeallocationAllocator->GetBlockSize())
	{
		for (FixedAllocator& Allocator : Allocators)
		{
			if (Allocator.GetBlockSize() >= SizeOfMemoryToDealloc)
			{
				LastDeallocationAllocator = &Allocator;
				break;
			}
		}
	}

	assert(LastDeallocationAllocator != nullptr);
	assert(LastDeallocationAllocator->GetBlockSize() >= SizeOfMemoryToDealloc);
	LastDeallocationAllocator->Dealloc(MemoryToDealloc);
}
