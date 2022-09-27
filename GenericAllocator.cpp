// Alessandro Pegoraro, 2022

#include "GenericAllocator.h"

#include <cassert>
#include <iostream>


using namespace APMemory;


GenericAllocator::GenericAllocator(const std::size_t TotalMemoryAvailable)
{
	// The first block has size equal to the TotalMemoryAvailable parameter.
	// However, malloc allocates more memory in order to take into account the first and last block overhead.
	FirstBlock = static_cast<MemControlBlock*>(malloc(2 * sizeof(MemControlBlock) + TotalMemoryAvailable));

	if (FirstBlock == nullptr)
	{
		std::cerr << "Error: Failed to allocate memory.\n";
		return;
	}

	// Damn you, pointer arithmetics!
	{
		char* const EndOfFirstBlock = (char*)(FirstBlock + 1);
		LastBlock = (MemControlBlock*)(EndOfFirstBlock + TotalMemoryAvailable);
	}

	FirstBlock->IsAvailable = true;
	LastBlock->IsAvailable = false;

	FirstBlock->Next = LastBlock;
	LastBlock->Next = nullptr;

	FirstBlock->Previous = nullptr;
	LastBlock->Previous = FirstBlock;

	std::cout << "GetSize() = " << FirstBlock->GetSize() << ", ";
	std::cout << "sizeof(MemControlBlock) = " << sizeof(MemControlBlock) << "\n\n";

	std::cout << "Init!\n\n";
}

GenericAllocator::~GenericAllocator()
{
	free(FirstBlock);

	std::cout << "Deinit!\n";
}

// Uses worst fit algorithm.
void* GenericAllocator::Alloc(const std::size_t BytesToAlloc)
{
	// EmptyBlock->Next is specificaly chosen to make GetSize() return 0.
	static MemControlBlock EmptyBlock = { &EmptyBlock + 1, nullptr, false };

	std::cout << "Bytes to allocate: " << BytesToAlloc << " ";
	std::cout << "Numero totale di blocchi: " << GetNumberOfMemBlocks() << "\n";

	MemControlBlock* CurrentBlock = FirstBlock;
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

void GenericAllocator::Dealloc(void* SpaceToDealloc)
{
	if (SpaceToDealloc == nullptr)
	{
		std::cerr << "Error: Trying to Dealloc a nullptr.\n";
		return;
	}

	MemControlBlock* const BlockToDealloc = static_cast<MemControlBlock*>(SpaceToDealloc) - 1;

	const MemControlBlock* const Previous = BlockToDealloc->Previous;
	const MemControlBlock* const Next = BlockToDealloc->Next;

	const bool PreviousAvailable = Previous != nullptr ? Previous->IsAvailable : false;
	const bool NextAvailable = Next != nullptr ? Next->IsAvailable : false;

	const bool BothAvailable = PreviousAvailable && NextAvailable;
	const bool OnlyOneAvailable = PreviousAvailable != NextAvailable;

	// Maybe these if - elif blocks are redundant, but they are highly readable... I hope :p

	if (OnlyOneAvailable)
	{
		if (PreviousAvailable)
		{
			MergeMemBlockWithItsPrevious(BlockToDealloc);
		}
		else
		{
			BlockToDealloc->IsAvailable = true;
			MergeMemBlockWithItsNext(BlockToDealloc);
		}
	}

	else if (BothAvailable)
	{
		// Choose the one with the greater size, and merge with it.
		if (Previous->GetSize() >= Next->GetSize())
		{
			MergeMemBlockWithItsPrevious(BlockToDealloc);
		}
		else
		{
			BlockToDealloc->IsAvailable = true;
			MergeMemBlockWithItsNext(BlockToDealloc);
		}
	}

	else
	{
		// No merging, just "freeing" the block.
		BlockToDealloc->IsAvailable = true;
	}
}

int GenericAllocator::GetNumberOfMemBlocks() const
{
	int NumberOfBlocks = 0;
	const MemControlBlock* iBlock = FirstBlock;

	while (iBlock != nullptr)
	{
		++NumberOfBlocks;
		iBlock = iBlock->Next;
	}

	return NumberOfBlocks - 1; // -1, because the last block is counted, which in reality doesn't exist.
}
