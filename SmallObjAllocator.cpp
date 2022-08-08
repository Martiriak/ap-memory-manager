#include "SmallObjAllocator.h"

#include <cstdlib>
#include <cmath>
#include <cassert>

namespace APMemory
{
	//SmallObjAllocator::MinimumNumberOfBlocksInChunks = 8;

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



	void FixedAllocator::Init(const std::size_t ChunksBlockSize, const unsigned char ChunksBlocksNumber)
	{
		BlockSize = ChunksBlockSize;
		BlocksNumber = ChunksBlocksNumber;
	}

	void FixedAllocator::Reset()
	{
		for (Chunk& iChunk : Chunks)
		{
			iChunk.Shutdown();
		}

		Chunks.clear();
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
		assert(DeallocChunk != nullptr);

		unsigned char* const BytePointerToDeallocMemory = static_cast<unsigned char*>(MemoryToDealloc);

		if (BytePointerToDeallocMemory >= DeallocChunk->Data
			&& BytePointerToDeallocMemory < DeallocChunk->Data + (BlockSize * BlocksNumber))
		{
			const std::size_t DeallocChunkIndex = DeallocChunk - Chunks.data();
			
			bool NoMoreChunksRight = false;
			bool NoMoreChunksLeft = false;

			for (std::size_t i = 1; !NoMoreChunksRight || !NoMoreChunksLeft; ++i)
			{
				const std::size_t iRightChunkIndex = DeallocChunkIndex + i;
				const std::size_t iLeftChunkIndex = DeallocChunkIndex - i;

				NoMoreChunksRight = iRightChunkIndex > Chunks.size() - 1;
				NoMoreChunksLeft = iLeftChunkIndex < 0;

				if (!NoMoreChunksRight)
				{
					if (BytePointerToDeallocMemory >= Chunks[iRightChunkIndex].Data
						&& BytePointerToDeallocMemory < Chunks[iRightChunkIndex].Data + (BlockSize * BlocksNumber))
					{
						DeallocChunk = &Chunks[iRightChunkIndex];
						break;
					}
				}

				if (!NoMoreChunksLeft)
				{
					if (BytePointerToDeallocMemory >= Chunks[iLeftChunkIndex].Data
						&& BytePointerToDeallocMemory < Chunks[iLeftChunkIndex].Data + (BlockSize * BlocksNumber))
					{
						DeallocChunk = &Chunks[iLeftChunkIndex];
						break;
					}
				}
			}
		}

		DeallocChunk->Dealloc(MemoryToDealloc, BlockSize);

		if (DeallocChunk->NumberOfAvailableBlocks == 0)
		{
			const std::size_t DeallocChunkIndex = DeallocChunk - Chunks.data();

			Chunk TmpSwapVar = Chunks[Chunks.size() - 1];
			Chunks[Chunks.size() - 1] = Chunks[DeallocChunkIndex];
			Chunks[DeallocChunkIndex] = TmpSwapVar;
			
			DeallocChunk = AllocChunk;

			if (Chunks[Chunks.size() - 2].NumberOfAvailableBlocks == 0)
			{
				Chunk DiscardedChunk = Chunks.back();
				DiscardedChunk.Shutdown();
				Chunks.pop_back();
			}
		}
	}



	SmallObjAllocator::SmallObjAllocator(const std::size_t ChunkSize, const std::size_t MaxObjectSize)
	{
		assert(MaxObjectSize % 2 == 0);

		Allocators.reserve(std::log2(MaxObjectSize));

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

	void* SmallObjAllocator::Alloc(const std::size_t BytesToAlloc)
	{
		if (BytesToAlloc == 0 || BytesToAlloc > Allocators.back().GetBlockSize()) return nullptr;

		if (AllocAllocator == nullptr || BytesToAlloc > AllocAllocator->GetBlockSize())
		{
			for (FixedAllocator& Allocator : Allocators)
			{
				if (Allocator.GetBlockSize() >= BytesToAlloc)
				{
					AllocAllocator = &Allocator;
					DeallocAllocator = &Allocator;
					break;
				}
			}
		}

		assert(AllocAllocator != nullptr);
		assert(AllocAllocator->GetBlockSize() >= BytesToAlloc);
		return AllocAllocator->Alloc();
	}

	void SmallObjAllocator::Dealloc(void* MemoryToDealloc, const std::size_t SizeOfMemoryToDealloc)
	{
		if (MemoryToDealloc == nullptr) return;

		if (DeallocAllocator == nullptr || SizeOfMemoryToDealloc > DeallocAllocator->GetBlockSize())
		{
			for (FixedAllocator& Allocator : Allocators)
			{
				if (Allocator.GetBlockSize() >= SizeOfMemoryToDealloc)
				{
					DeallocAllocator = &Allocator;
					break;
				}
			}
		}

		assert(DeallocAllocator != nullptr);
		assert(DeallocAllocator->GetBlockSize() >= SizeOfMemoryToDealloc);
		DeallocAllocator->Dealloc(MemoryToDealloc);
	}
}
