// Alessandro Pegoraro, 2022

#include "SmallObjAllocator.h"

#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream> // For debug

using namespace APMemory;


SmallObjAllocator::SmallObjAllocator(const std::size_t ChunkSize, const std::size_t MaxObjectSize)
{
	assert(MaxObjectSize % 2 == 0);

	std::cout << "================================================\nBuilding Small Obj Allocator...\n";

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

	std::cout << "Created " << Allocators.size() << " pools of memory: ";
	for (const FixedAllocator& Allocator : Allocators)
	{
		std::cout << Allocator.GetBlockSize() << ", ";
	}
	std::cout << "all in bytes.\n================================================\n\n";
}

SmallObjAllocator::~SmallObjAllocator()
{
	std::cout << "================================================\nCleaning Small Obj Allocator...\n";

	for (FixedAllocator& Allocator : Allocators)
	{
		Allocator.Reset();
	}

	Allocators.clear();
	std::cout << "================================================\n\n";
}

void* SmallObjAllocator::Alloc(const std::size_t BytesToAlloc)
{
	if (BytesToAlloc == 0 || BytesToAlloc > Allocators.back().GetBlockSize()) return nullptr;

	if (LastAllocationAllocator == nullptr || BytesToAlloc > LastAllocationAllocator->GetBlockSize()
		|| BytesToAlloc <= LastAllocationAllocator->GetBlockSize() / 2)
	{
		std::cout << "----> Searching Available Allocator... ";
		int debugCount = 0;

		for (FixedAllocator& Allocator : Allocators)
		{
			++debugCount;
			if (Allocator.GetBlockSize() >= BytesToAlloc)
			{
				std::cout << "Found after " << debugCount << " cycles.\n";
				std::cout << "      " << BytesToAlloc << '/' << Allocator.GetBlockSize() << " (ObjSize / BlocksSize)\n\n";

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

	if (LastDeallocationAllocator == nullptr || SizeOfMemoryToDealloc > LastDeallocationAllocator->GetBlockSize()
		|| SizeOfMemoryToDealloc <= LastDeallocationAllocator->GetBlockSize() / 2)
	{
		std::cout << "----> Searching Correct Allocator... ";
		int debugCount = 0;

		for (FixedAllocator& Allocator : Allocators)
		{
			++debugCount;
			if (Allocator.GetBlockSize() >= SizeOfMemoryToDealloc)
			{
				std::cout << "Found after " << debugCount << " cycles.\n";
				std::cout << "      " << SizeOfMemoryToDealloc << '/' << Allocator.GetBlockSize() << " (ObjSize / BlocksSize)\n\n";

				LastDeallocationAllocator = &Allocator;
				break;
			}
		}
	}

	assert(LastDeallocationAllocator != nullptr);
	assert(LastDeallocationAllocator->GetBlockSize() >= SizeOfMemoryToDealloc);
	LastDeallocationAllocator->Dealloc(MemoryToDealloc);
}
