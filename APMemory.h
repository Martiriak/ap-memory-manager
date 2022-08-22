// Alessandro Pegoraro, 2022

#pragma once

#include <cstddef>
//#include <iostream>

//536870912 = 512 MB

namespace APMemory
{
	// Not sure if I will keep this, currently unused.
	enum class AllocatorType : char
	{
		Generic,
		SmallObjects
	};

	/** Enables the use of APMemory functions. 
	* @param TotalMemoryAvailable the amount of memory to allocate.
	*/
	void InitMemoryManager(const std::size_t TotalMemoryAvailable);

	/** Deallocates all the memory previously allocated, and disables the use of APMemory functions. */
	void ShutdownMemoryManager();

	/** Substitutes malloc(). */
	void* Alloc(const std::size_t BytesToAlloc);

	/** Substitutes free(). */
	void Dealloc(void* SpaceToDealloc);


	/** Utility function to emulate the delete operator. */
	template<typename T>
	inline void Delete(T* ObjToDealloc)
	{
		if (ObjToDealloc != nullptr)
		{
			ObjToDealloc->~T();
			Dealloc(ObjToDealloc);
		}
	}

	int GetNumberOfMemBlocks();

} // APMemory End




// Appunti random:
// L'uso di classi vuote come "tags" non è una idea malvagia, dato che secondo 
// Bjarne Stroustrup esiste una regola di ottimizzazione: https://www.stroustrup.com/bs_faq2.html#sizeof-empty
// 
//
