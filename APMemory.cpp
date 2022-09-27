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
	const std::size_t ChunkSize = 16;
	const std::size_t MaxSmallObjSize = 1024;

	const std::size_t GenericMemoryAvailable = 1024 * 12; // Will be removed after the rework on the generic allocator.

	unsigned char bAtExitRegistered = false;

	GenericAllocator* GenAlloc = nullptr;
	SmallObjAllocator* SmallAlloc = nullptr;

	void ShutdownMemoryManager();

	inline void LazyInitGenericAllocator()
	{
		if (GenAlloc != nullptr) return;

		GenAlloc = static_cast<GenericAllocator*>(malloc(sizeof(GenericAllocator)));

		if (GenAlloc == nullptr)
		{
			std::cerr << "Error: Failed to initialize generic allocator.\n";
			return;
		}

		GenAlloc = new(GenAlloc) GenericAllocator(GenericMemoryAvailable);
	}

	inline void LazyInitSmallObjAllocator()
	{
		if (SmallAlloc != nullptr) return;

		SmallAlloc = static_cast<SmallObjAllocator*>(malloc(sizeof(SmallObjAllocator)));

		if (SmallAlloc == nullptr)
		{
			std::cerr << "Error: Failed to initialize small object allocator.\n";
			return;
		}

		SmallAlloc = new(SmallAlloc) SmallObjAllocator(ChunkSize, MaxSmallObjSize);
	}

	inline void RegisterExitIfNotAlready()
	{
		if (bAtExitRegistered) return;

		const int Result = std::atexit(ShutdownMemoryManager);

		if (Result != 0)
		{
			std::cout << "\n\n\t!!Something went wrong with registration!!\n\n";
			return;
		}

		bAtExitRegistered = true;
	}
}


#ifdef APMEMORY_OVERRIDE_DEFAULT_NEW_DELETE
void* operator new(std::size_t Size)
{
	return APMemory::Alloc(Size);
}

void operator delete(void* PtrToMemory, std::size_t ObjSize)
{
	APMemory::Dealloc(PtrToMemory, ObjSize);
}
#endif



void* APMemory::Alloc(const std::size_t BytesToAlloc)
{
	RegisterExitIfNotAlready();

	if (BytesToAlloc > MaxSmallObjSize)
	{
		LazyInitGenericAllocator();

		if (GenAlloc != nullptr) return GenAlloc->Alloc(BytesToAlloc);
	}
	else
	{
		LazyInitSmallObjAllocator();

		if (SmallAlloc != nullptr) return SmallAlloc->Alloc(BytesToAlloc);
	}

	return nullptr;
}


void APMemory::Dealloc(void* SpaceToDealloc, const std::size_t ObjectSize)
{
	if (ObjectSize > MaxSmallObjSize)
	{
		if (GenAlloc != nullptr)
		{
			GenAlloc->Dealloc(SpaceToDealloc);
			return;
		}
	}
	else
	{
		if (SmallAlloc != nullptr) 
		{
			SmallAlloc->Dealloc(SpaceToDealloc, ObjectSize);
			return;
		}
	}

	std::cerr << "Error: Attempted deallocation of unallocated memory.\n";
}


int APMemory::GetNumberOfMemBlocks()
{
	assert(GenAlloc != nullptr);
	return GenAlloc->GetNumberOfMemBlocks();
}


namespace
{
	void ShutdownMemoryManager()
	{
		if (GenAlloc != nullptr)
		{
			GenAlloc->~GenericAllocator();
			free(GenAlloc);
			GenAlloc = nullptr;
		}

		if (SmallAlloc != nullptr)
		{
			SmallAlloc->~SmallObjAllocator();
			free(SmallAlloc);
			SmallAlloc = nullptr;
		}

		std::cout << "\n\n\t!!Ripulito!!\n\n";
	}
}
