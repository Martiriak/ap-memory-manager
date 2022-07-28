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

	private:

		std::size_t BlockSize = 0;
		unsigned char BlocksNumber = 0;
		std::vector<Chunk> Chunks;
		Chunk* AllocChunk = nullptr;
		Chunk* DeallocChunk = nullptr;

		void* Alloc();
		void Dealloc(void* MemoryToDealloc);
	};
}
