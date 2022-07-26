#pragma once

#include <cstddef>
//#include <iostream>

//536870912 = 512 MB

namespace APMemory
{
	enum class AllocatorType : char
	{
		Generic,
		SmallObjects
	};


	void InitMemoryManager(const std::size_t TotalMemoryAvailable);
	void ShutdownMemoryManager();

	/** Uses Worst-Fit algorithm. */
	void* Alloc(const std::size_t BytesToAlloc);

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
