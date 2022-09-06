// Alessandro Pegoraro, 2022

#pragma once

#include <cstddef>

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

	int GetNumberOfMemBlocks();

} // APMemory End
