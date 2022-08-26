// Alessandro Pegoraro, 2022

#include "SmallObjAllocator.h"

#include <cstdlib>
#include <cmath>
#include <cassert>

#include <iostream>

using namespace APMemory;

void SmallObjAllocator::FixedAllocator::Init(const std::size_t ChunksBlockSize, const unsigned char ChunksBlocksNumber)
{
	BlockSize = ChunksBlockSize;
	BlocksNumber = ChunksBlocksNumber;
}

void SmallObjAllocator::FixedAllocator::Reset()
{
	std::cout << "Resetting Allocator of size " << GetBlockSize() << '\n';
	for (Chunk& iChunk : Chunks)
	{
		iChunk.Shutdown();
	}

	Chunks.clear();

	std::cout << '\n';
}

void* SmallObjAllocator::FixedAllocator::Alloc()
{
	if (LastAllocationChunk == nullptr || LastAllocationChunk->NumberOfAvailableBlocks == 0)
	{
		LastAllocationChunk = nullptr;

		std::cout << "-------> Searching Available Chunk... ";
		int debugCount = 0;

		// Find chunks available to allocate.
		for (Chunk& iChunk : Chunks)
		{
			++debugCount;

			if (iChunk.NumberOfAvailableBlocks != 0)
			{
				std::cout << "Found after " << debugCount << " cycles.\n";

				LastAllocationChunk = &iChunk;
				break;
			}
		}

		// If there's none, make one.
		if (LastAllocationChunk == nullptr)
		{
			std::cout << "Not found, creating one.\n";

			Chunk NewChunk;
			NewChunk.Init(BlockSize, BlocksNumber);

			Chunks.reserve(Chunks.size() + 1);
			Chunks.push_back(NewChunk);

			LastAllocationChunk = &Chunks.back();
			LastDeallocationChunk = &Chunks.back();
		}
	}

	assert(LastAllocationChunk != nullptr);
	assert(LastAllocationChunk->NumberOfAvailableBlocks > 0);
	return LastAllocationChunk->Alloc(BlockSize);
}

void SmallObjAllocator::FixedAllocator::Dealloc(void* MemoryToDealloc)
{
	assert(LastDeallocationChunk != nullptr);

	unsigned char* const PointerToMemoryToDealloc = static_cast<unsigned char*>(MemoryToDealloc);

	// If memory to deallocate is not in the LastDeallocationChunk...
	if (PointerToMemoryToDealloc < LastDeallocationChunk->Data
		|| PointerToMemoryToDealloc >= LastDeallocationChunk->Data + (BlockSize * BlocksNumber))
	{
		std::cout << "-------> Searching Correct Chunk... ";

		const std::size_t LastDeallocationChunkIndex = LastDeallocationChunk - Chunks.data();

		bool NoMoreChunksRight = false;
		bool NoMoreChunksLeft = false;

		// Search for the correct chunk, checking both the left side and the right side at each step.
		for (std::size_t i = 1; !NoMoreChunksRight || !NoMoreChunksLeft; ++i)
		{
			const std::size_t iRightChunk = LastDeallocationChunkIndex + i;
			const std::size_t iLeftChunk = LastDeallocationChunkIndex - i;

			NoMoreChunksRight = iRightChunk > Chunks.size() - 1;
			NoMoreChunksLeft = iLeftChunk < 0;

			if (!NoMoreChunksRight)
			{
				if (PointerToMemoryToDealloc >= Chunks[iRightChunk].Data
					&& PointerToMemoryToDealloc < Chunks[iRightChunk].Data + (BlockSize * BlocksNumber))
				{
					std::cout << "Found it " << i << " steps on the right.\n";
					LastDeallocationChunk = &Chunks[iRightChunk];
					break;
				}
			}

			if (!NoMoreChunksLeft)
			{
				if (PointerToMemoryToDealloc >= Chunks[iLeftChunk].Data
					&& PointerToMemoryToDealloc < Chunks[iLeftChunk].Data + (BlockSize * BlocksNumber))
				{
					std::cout << "Found it " << i << " steps on the left.\n";
					LastDeallocationChunk = &Chunks[iLeftChunk];
					break;
				}
			}
		}
	}

	LastDeallocationChunk->Dealloc(MemoryToDealloc, BlockSize);

	// If the chunk is now empty...
	if (LastDeallocationChunk->NumberOfAvailableBlocks == BlocksNumber)
	{
		std::cout << "         !! Chunk has been emptied!";

		// ...move the now empty chunk at the end.
		const std::size_t LastDeallocationChunkIndex = LastDeallocationChunk - Chunks.data();

		std::cout << " Empty chunk index: " << LastDeallocationChunkIndex;

		Chunk TmpSwapVar = Chunks[Chunks.size() - 1];
		Chunks[Chunks.size() - 1] = Chunks[LastDeallocationChunkIndex];
		Chunks[LastDeallocationChunkIndex] = TmpSwapVar;

		// If there are 2 empty chunkes, remove the last.
		if (Chunks.size() > 1 && Chunks[Chunks.size() - 2].NumberOfAvailableBlocks == BlocksNumber)
		{
			std::cout << " Another chunk was already empty, so deleting this one.";

			Chunk DiscardedChunk = Chunks.back();
			DiscardedChunk.Shutdown();
			Chunks.pop_back();

			LastDeallocationChunk = &(Chunks.back());

			if (LastAllocationChunk - Chunks.data() >= Chunks.size())
			{
				LastAllocationChunk = LastDeallocationChunk;
			}
		}

		std::cout << '\n';
	}
}
