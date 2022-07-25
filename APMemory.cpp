#include "APMemory.h"

#include <cstdlib>
#include <iostream>

namespace
{
	bool IsInitialized = false;

	APMemory::MemControlBlock* FirstBlock = nullptr;
	APMemory::MemControlBlock* LastBlock = nullptr;

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
		if (IsInitialized)
		{
			std::cerr << "Error: Manager Already Initialized!\n";
			return;
		}

		FirstBlock = static_cast<MemControlBlock*>(malloc(2 * sizeof(MemControlBlock) + TotalMemoryAvailable));

		if (FirstBlock == nullptr)
		{
			std::cerr << "Error: Failed to allocate memory.\n";
			return;
		}

		{
			char* const EndOfFirstBlock = (char*) (FirstBlock + 1);
			LastBlock = (MemControlBlock*) (EndOfFirstBlock + TotalMemoryAvailable);
		}

		FirstBlock->IsAvailable = true;
		LastBlock->IsAvailable = false;

		FirstBlock->Next = LastBlock;
		LastBlock->Next = nullptr;

		FirstBlock->Previous = nullptr;
		LastBlock->Previous = FirstBlock;

		std::cout << "GetSize() = " << FirstBlock->GetSize() << "\n";
		std::cout << "sizeof(MemControlBlock) = " << sizeof(MemControlBlock) << "\n\n";

		std::cout << "Init!\n\n";
		IsInitialized = true;
	}

	void ShutdownMemoryManager()
	{
		if (!IsInitialized)
		{
			std::cerr << "Error: Attempted Shutdown of Uninitialized Manager!\n";
			return;
		}

		free(FirstBlock);
		FirstBlock = nullptr;
		LastBlock = nullptr;

		std::cout << "Deinit!\n";
		IsInitialized = false;
	}

	void* Alloc(const std::size_t BytesToAlloc)
	{
		// EmptyBlock->Next is specificaly chosen to make GetSize() return 0.
		static MemControlBlock EmptyBlock = { false, &EmptyBlock + 1, nullptr};

		std::cout << "Bytes to allocate: " << BytesToAlloc << " ";
		std::cout << "Numero totale di blocchi: " << GetNumberOfMemBlocks() << "\n";

		MemControlBlock* CurrentBlock = FirstBlock;
		MemControlBlock* WorstFitBlock = &EmptyBlock;

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

		if (WorstFitBlock->GetSize() > 0)
		{
			MemControlBlock* NewNextBlock;

			// Initialize NewNextBlock pointer to point to the start of the newly created block.
			{
				MemControlBlock* const teddyBear = WorstFitBlock + 1;
				char* const NewNextBlockPointer = (char*) teddyBear + BytesToAlloc;
				NewNextBlock = (MemControlBlock*) NewNextBlockPointer;
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
			BlockToDealloc->IsAvailable = true;
		}
	}

	int GetNumberOfMemBlocks()
	{
		int NumberOfBlocks = 0;
		MemControlBlock* iBlock = FirstBlock;

		while (iBlock != nullptr)
		{
			++NumberOfBlocks;
			iBlock = iBlock->Next;
		}
		
		return NumberOfBlocks - 1;
	}


} // APMemory End
