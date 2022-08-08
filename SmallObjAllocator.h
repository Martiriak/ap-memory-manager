#pragma once

#include <cstdlib>
#include <vector>

namespace APMemory
{
	struct Chunk
	{
		unsigned char* Data = nullptr;

		unsigned char FirstAvailableBlockIndex = 0;
		unsigned char NumberOfAvailableBlocks = 0;

		void Init(const std::size_t BlockSize, const unsigned char BlocksNumber);
		void Reset(const std::size_t BlockSize, const unsigned char BlocksNumber);
		void Shutdown();
		void* Alloc(const std::size_t BlockSize);
		void Dealloc(void* BlockToDealloc, const std::size_t BlockSize);
	};

	class FixedAllocator
	{

	public:

		void Init(const std::size_t ChunksBlockSize, const unsigned char ChunksBlocksNumber);
		void Reset();

		inline std::size_t GetBlockSize() const { return BlockSize; }

		void* Alloc();
		void Dealloc(void* MemoryToDealloc);

	private:

		std::size_t BlockSize = 0;
		unsigned char BlocksNumber = 0;
		std::vector<Chunk> Chunks;
		Chunk* AllocChunk = nullptr;
		Chunk* DeallocChunk = nullptr;
	};

	class SmallObjAllocator
	{

	public:

		SmallObjAllocator(const std::size_t ChunkSize, const std::size_t MaxObjectSize);

		void* Alloc(const std::size_t BytesToAlloc);
		void Dealloc(void* MemoryToDealloc, const std::size_t SizeOfMemoryToDealloc);

		static const unsigned char MinimumNumberOfBlocksInChunks = 8;

	private:

		std::vector<FixedAllocator> Allocators;
		FixedAllocator* AllocAllocator = nullptr;
		FixedAllocator* DeallocAllocator = nullptr;
	};
}
