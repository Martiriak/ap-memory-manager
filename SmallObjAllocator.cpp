#include "SmallObjAllocator.h"

#include <cstdlib>
#include <cassert>

namespace APMemory
{
	void Chunk::Init(const std::size_t BlockSize, const unsigned char BlocksNumber)
	{
		assert(Data == nullptr);
		Data = static_cast<unsigned char*>(malloc(BlockSize * BlocksNumber));

		Reset(BlockSize, BlocksNumber);
	}

	void Chunk::Reset(const std::size_t BlockSize, const unsigned char BlocksNumber)
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

	void Chunk::Shutdown()
	{
		assert(Data != nullptr);
		free(Data);
		Data = nullptr;
	}

	void* Chunk::Alloc(const std::size_t BlockSize)
	{
		assert(Data != nullptr);
		//if (NumberOfAvailableBlocks == 0) return nullptr; Not necessary to perform this check.

		unsigned char* const NewlyAllocatedBlock = Data + (FirstAvailableBlockIndex * BlockSize);

		FirstAvailableBlockIndex = *NewlyAllocatedBlock;
		--NumberOfAvailableBlocks;

		return NewlyAllocatedBlock;
	}

	void Chunk::Dealloc(void* BlockToDealloc, const std::size_t BlockSize)
	{
		assert(Data != nullptr);
		//if (BlockToDealloc == nullptr) return; Not necessary to perform this check.

		unsigned char* const NewlyDeallocatedBlock = static_cast<unsigned char*>(BlockToDealloc);

		*NewlyDeallocatedBlock = FirstAvailableBlockIndex;
		++NumberOfAvailableBlocks;
		FirstAvailableBlockIndex = static_cast<unsigned char>((NewlyDeallocatedBlock - Data) / BlockSize);
	}



	void* FixedAllocator::Alloc()
	{
		if (AllocChunk == nullptr || AllocChunk->NumberOfAvailableBlocks == 0)
		{
			AllocChunk = nullptr;

			for (Chunk& iChunk : Chunks)
			{
				if (iChunk.NumberOfAvailableBlocks != 0)
				{
					AllocChunk = &iChunk;
					break;
				}
			}

			if (AllocChunk == nullptr)
			{
				Chunk NewChunk;
				NewChunk.Init(BlockSize, BlocksNumber);

				Chunks.reserve(Chunks.size() + 1);
				Chunks.push_back(NewChunk);

				AllocChunk = &Chunks.back();
				DeallocChunk = &Chunks.back();
			}
		}

		assert(AllocChunk != nullptr);
		assert(AllocChunk->NumberOfAvailableBlocks > 0);
		return AllocChunk->Alloc(BlockSize);
	}

	void FixedAllocator::Dealloc(void* MemoryToDealloc)
	{

	}
}
