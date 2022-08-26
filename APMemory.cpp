// Alessandro Pegoraro, 2022

#include "APMemory.h"

#include <cstdlib>
#include <cassert>
#include <iostream>

#include "GenericAllocator.h"
#include "SmallObjAllocator.h"


using namespace APMemory;

namespace
{
	GenericAllocator* GenAlloc = nullptr;
	SmallObjAllocator* SmallAlloc = nullptr;
}


void APMemory::InitMemoryManager(const std::size_t TotalMemoryAvailable)
{
	if (GenAlloc != nullptr)
	{
		std::cerr << "Error: Memory Manager already initialized!\n";
		return;
	}

	GenAlloc = static_cast<GenericAllocator*>(malloc(sizeof(GenericAllocator)));

	if (GenAlloc == nullptr)
	{
		std::cerr << "Error: Failed to initialize Memory Manager.\n";
		return;
	}

	GenAlloc = new(GenAlloc) GenericAllocator(TotalMemoryAvailable);
}

void APMemory::ShutdownMemoryManager()
{
	if (GenAlloc == nullptr)
	{
		std::cerr << "Error: Attempting to shutdown uninitialized Memory Manager!\n";
		return;
	}

	if (SmallAlloc != nullptr)
	{
		SmallAlloc->~SmallObjAllocator();
		free(SmallAlloc);
		SmallAlloc = nullptr;
	}

	GenAlloc->~GenericAllocator();
	free(GenAlloc);
	GenAlloc = nullptr;
}



void* APMemory::Alloc(const std::size_t BytesToAlloc)
{
	/*assert(GenAlloc != nullptr);
	return GenAlloc->Alloc(BytesToAlloc);*/

	if (SmallAlloc == nullptr)
	{
		SmallAlloc = new(malloc(sizeof(SmallObjAllocator))) SmallObjAllocator(16, 1024);

		if (SmallAlloc == nullptr)
		{
			std::cerr << "Error: Failed to initialize small object allocator.\n";
			return nullptr;
		}
	}

	return SmallAlloc->Alloc(BytesToAlloc);
}


void APMemory::Dealloc(void* SpaceToDealloc, const std::size_t ObjectSize)
{
	/*assert(GenAlloc != nullptr);
	GenAlloc->Dealloc(SpaceToDealloc);*/

	if (SmallAlloc == nullptr)
	{
		std::cerr << "Error: Attempted deallocation of nothing.\n";
		return;
	}

	SmallAlloc->Dealloc(SpaceToDealloc, ObjectSize);
}


int APMemory::GetNumberOfMemBlocks()
{
	assert(GenAlloc != nullptr);
	return GenAlloc->GetNumberOfMemBlocks();
}
