#include "APMemory.h"

#include <cstdlib>
#include <cassert>
#include <iostream>

#include "MemControlBlock.h"
#include "MemManagerInfo.h"

namespace
{
	// Contains pointers to first and last (guard) memory blocks.
	APMemory::MemManagerInfo* MemInfo = nullptr;

	void MergeCurrentWithItsPrevious(APMemory::MemControlBlock* Current)
	{
		Current->Previous->Next = Current->Next;
		Current->Next->Previous = Current->Previous;
	}

	void MergeCurrentWithItsNext(APMemory::MemControlBlock* Current)
	{
		MergeCurrentWithItsPrevious(Current->Next);
	}

} // Anonymous Namespace End



namespace APMemory
{
	void InitMemoryManager(const std::size_t TotalMemoryAvailable)
	{
		if (MemInfo != nullptr)
		{
			std::cerr << "Error: Manager Already Initialized!\n";
			return;
		}

		MemInfo = static_cast<MemManagerInfo*>(malloc(sizeof(MemManagerInfo)));

		if (MemInfo == nullptr)
		{
			std::cerr << "Error: Failed to allocate memory.\n";
			return;
		}

		// The first block has size equal to the TotalMemoryAvailable parameter.
		// However, malloc allocates more memory in order to take into account the first and last block overhead.
		MemInfo->FirstBlock = static_cast<MemControlBlock*>(malloc(2 * sizeof(MemControlBlock) + TotalMemoryAvailable));

		if (MemInfo->FirstBlock == nullptr)
		{
			std::cerr << "Error: Failed to allocate memory.\n";
			return;
		}

		// Damn you, pointer arithmetics!
		{
			char* const EndOfFirstBlock = (char*) (MemInfo->FirstBlock + 1);
			MemInfo->LastBlock = (MemControlBlock*) (EndOfFirstBlock + TotalMemoryAvailable);
		}

		MemInfo->FirstBlock->IsAvailable = true;
		MemInfo->LastBlock->IsAvailable = false;

		MemInfo->FirstBlock->Next = MemInfo->LastBlock;
		MemInfo->LastBlock->Next = nullptr;

		MemInfo->FirstBlock->Previous = nullptr;
		MemInfo->LastBlock->Previous = MemInfo->FirstBlock;

		std::cout << "GetSize() = " << MemInfo->FirstBlock->GetSize() << ", ";
		std::cout << "sizeof(MemControlBlock) = " << sizeof(MemControlBlock) << "\n\n";

		std::cout << "Init!\n\n";
	}

	void ShutdownMemoryManager()
	{
		if (MemInfo == nullptr)
		{
			std::cerr << "Error: Attempted Shutdown of Uninitialized Manager!\n";
			return;
		}

		free(MemInfo->FirstBlock);
		free(MemInfo);
		MemInfo = nullptr;

		std::cout << "Deinit!\n";
	}

	// Uses worst fit algorithm.
	void* Alloc(const std::size_t BytesToAlloc)
	{
		// EmptyBlock->Next is specificaly chosen to make GetSize() return 0.
		static MemControlBlock EmptyBlock = { false, &EmptyBlock + 1, nullptr};

		assert(MemInfo != nullptr);

		std::cout << "Bytes to allocate: " << BytesToAlloc << " ";
		std::cout << "Numero totale di blocchi: " << GetNumberOfMemBlocks() << "\n";

		MemControlBlock* CurrentBlock = MemInfo->FirstBlock;
		MemControlBlock* WorstFitBlock = &EmptyBlock;

		// Debug Variables
		int count = 0;
		int blockFound = 0;

		while (CurrentBlock != nullptr)
		{
			++count;
			std::cout << "\tBlocco numero: " << count << " , GetSize(): " << CurrentBlock->GetSize() << ", ";

			if (CurrentBlock->IsAvailable)
			{
				std::cout << "Stato: Disponibile!";

				const std::size_t BlockSize = CurrentBlock->GetSize();

				if (BlockSize >= BytesToAlloc + sizeof(MemControlBlock) && BlockSize > WorstFitBlock->GetSize())
				{
					WorstFitBlock = CurrentBlock;

					blockFound = count;
					std::cout << "\n\t\t!Well fitted!";
				}
			}
			else
			{
				std::cout << "Stato: Occupato.";
			}

			std::cout << "\n";

			CurrentBlock = CurrentBlock->Next;
		}

		std::cout << "\t====Finished Search...====\tFound block " << blockFound << " of size: " << WorstFitBlock->GetSize() << '\n';

		// If a block has actually been found...
		if (WorstFitBlock->GetSize() > 0)
		{
			MemControlBlock* NewNextBlock;

			// Initialize NewNextBlock pointer to point to the start of the newly created block.
			{
				MemControlBlock* const EndOfFoundBlockOverhead = WorstFitBlock + 1;
				char* const NewNextBlockPointer = (char*)EndOfFoundBlockOverhead + BytesToAlloc;
				NewNextBlock = (MemControlBlock*)NewNextBlockPointer;
			}

			// If the next block already exists, we mustn't re-initialize it.
			if (BytesToAlloc != WorstFitBlock->GetSize())
			{
				std::cout << "\t\tPrepping a new block...\n";

				NewNextBlock->IsAvailable = true;
				NewNextBlock->Next = WorstFitBlock->Next;
				NewNextBlock->Previous = WorstFitBlock;
			}

			WorstFitBlock->IsAvailable = false;
			WorstFitBlock->Next = NewNextBlock;

			// Safe to do operation, even if the new next block already exists.
			if (NewNextBlock->Next != nullptr)
			{
				NewNextBlock->Next->Previous = NewNextBlock;
			}

			return WorstFitBlock + 1;
		}

		else return nullptr;
	}

	void Dealloc(void* SpaceToDealloc)
	{
		assert(MemInfo != nullptr);

		if (SpaceToDealloc == nullptr)
		{
			std::cerr << "Error: Trying to Dealloc a nullptr.\n";
			return;
		}
		
		MemControlBlock* BlockToDealloc = static_cast<MemControlBlock*>(SpaceToDealloc) - 1;

		MemControlBlock* Previous = BlockToDealloc->Previous;
		MemControlBlock* Next = BlockToDealloc->Next;

		bool PreviousAvailable = Previous != nullptr ? Previous->IsAvailable : false;
		bool NextAvailable = Next != nullptr ? Next->IsAvailable : false;

		bool BothAvailable = PreviousAvailable && NextAvailable;
		bool OnlyOneAvailable = PreviousAvailable != NextAvailable;

		// Maybe these if - elif blocks are redundant, but they are highly readable... I hope :p

		if (OnlyOneAvailable)
		{
			if (PreviousAvailable)
			{
				MergeCurrentWithItsPrevious(BlockToDealloc);
			}
			else
			{
				BlockToDealloc->IsAvailable = true;
				MergeCurrentWithItsNext(BlockToDealloc);
			}
		}

		else if (BothAvailable)
		{
			// Choose the one with the greater size, and merge with it.
			if (Previous->GetSize() >= Next->GetSize())
			{
				MergeCurrentWithItsPrevious(BlockToDealloc);
			}
			else
			{
				BlockToDealloc->IsAvailable = true;
				MergeCurrentWithItsNext(BlockToDealloc);
			}
		}

		else
		{
			// No merging, just "freeing" the block.
			BlockToDealloc->IsAvailable = true;
		}
	}

	int GetNumberOfMemBlocks()
	{
		int NumberOfBlocks = 0;
		MemControlBlock* iBlock = MemInfo->FirstBlock;

		while (iBlock != nullptr)
		{
			++NumberOfBlocks;
			iBlock = iBlock->Next;
		}
		
		return NumberOfBlocks - 1; // -1, because the last block is counted, which in reality doesn't exist.
	}


} // APMemory End
