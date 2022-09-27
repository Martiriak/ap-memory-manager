// Alessandro Pegoraro, 2022

#pragma once

#include <cstddef>
#include <new>

//536870912 = 512 MB

namespace APMemory
{
	/** Substitutes malloc(). */
	void* Alloc(const std::size_t BytesToAlloc);

	/** Substitutes free(). */
	void Dealloc(void* SpaceToDealloc, const std::size_t ObjectSize);


	/** Utility function to emulate the delete operator. */
	template<typename T>
	inline void Delete(T* ObjToDealloc)
	{
		if (ObjToDealloc != nullptr)
		{
			ObjToDealloc->~T();
			Dealloc(ObjToDealloc, sizeof(T));
		}
	}

} // APMemory End



#ifndef APMEMORY_DISABLE_NEW_DELETE_OVERRIDE
inline void* operator new(std::size_t Size)
{
	return APMemory::Alloc(Size);
}

inline void* operator new[](std::size_t Size)
{
	return APMemory::Alloc(Size);
}

inline void operator delete(void* PtrToMemory, std::size_t ObjSize)
{
	APMemory::Dealloc(PtrToMemory, ObjSize);
}

inline void operator delete[](void* PtrToMemory, std::size_t ObjSize)
{
	APMemory::Dealloc(PtrToMemory, ObjSize);
}
#endif
