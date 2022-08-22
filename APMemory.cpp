// Alessandro Pegoraro, 2022

#include "APMemory.h"

#include <cstdlib>
#include <cassert>
#include <iostream>

#include "GenericAllocator.h"


using namespace APMemory;

namespace
{
	GenericAllocator* GenAlloc = nullptr;
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

	GenAlloc->~GenericAllocator();
	free(GenAlloc);
	GenAlloc = nullptr;
}



void* APMemory::Alloc(const std::size_t BytesToAlloc)
{
	assert(GenAlloc != nullptr);
	return GenAlloc->Alloc(BytesToAlloc);
}


void APMemory::Dealloc(void* SpaceToDealloc)
{
	assert(GenAlloc != nullptr);
	GenAlloc->Dealloc(SpaceToDealloc);
}


int APMemory::GetNumberOfMemBlocks()
{
	assert(GenAlloc != nullptr);
	return GenAlloc->GetNumberOfMemBlocks();
}
