#pragma once

#include <cstdlib>
#include <vector>

namespace APMemory
{
	class SmallObjAllocator final
	{

	// Nested class definitions
	private:

		class FixedAllocator final
		{

		public:

			struct Chunk final
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

			void Init(const std::size_t ChunksBlockSize, const unsigned char ChunksBlocksNumber);
			void Reset();

			inline std::size_t GetBlockSize() const { return BlockSize; }

			void* Alloc();
			void Dealloc(void* MemoryToDealloc);

		private:

			std::size_t BlockSize = 0;
			unsigned char BlocksNumber = 0;
			std::vector<Chunk> Chunks;
			Chunk* LastAllocationChunk = nullptr;
			Chunk* LastDeallocationChunk = nullptr;
		};


	// SmallObjAllocator members
	public:

		SmallObjAllocator(const std::size_t ChunkSize, const std::size_t MaxObjectSize);
		~SmallObjAllocator();

		void* Alloc(const std::size_t BytesToAlloc);
		void Dealloc(void* MemoryToDealloc, const std::size_t SizeOfMemoryToDealloc);

		static const unsigned char MinimumNumberOfBlocksInChunks = 8;

	private:

		std::vector<FixedAllocator> Allocators;
		FixedAllocator* LastAllocationAllocator = nullptr;
		FixedAllocator* LastDeallocationAllocator = nullptr;
	};
}
