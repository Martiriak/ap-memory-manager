// Alessandro Pegoraro, 2022

#pragma once

#include <cstdlib>

namespace APMemory
{
	class GenericAllocator final
	{

	// Nested struct definitions
	private:

		struct MemControlBlock final
		{
			MemControlBlock* Next;
			MemControlBlock* Previous;
			bool IsAvailable;

			inline std::size_t GetSize() const
			{
				const char* const EndOfThisBlockOverhead = (char*)(this + 1);
				const char* const StartOfNextBlock = (char*)this->Next;

				return StartOfNextBlock - EndOfThisBlockOverhead;
			}
		};

	// GenericAllocator members
	public:

		GenericAllocator() = delete;
		explicit GenericAllocator(const std::size_t TotalMemoryAvailable);
		~GenericAllocator();

		void* Alloc(const std::size_t BytesToAlloc);
		void Dealloc(void* SpaceToDealloc);

		int GetNumberOfMemBlocks() const;

	private:

		/** The first block of all the allocated memory. */
		MemControlBlock* FirstBlock = nullptr;
		/** Intended as a guard, it is always marked as not available. */
		MemControlBlock* LastBlock = nullptr;


		inline static void MergeMemBlockWithItsPrevious(MemControlBlock* const Current)
		{
			Current->Previous->Next = Current->Next;
			Current->Next->Previous = Current->Previous;
		}

		inline static void MergeMemBlockWithItsNext(MemControlBlock* const Current)
		{
			MergeMemBlockWithItsPrevious(Current->Next);
		}
	};


}
