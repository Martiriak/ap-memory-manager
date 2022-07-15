#pragma once

#include <cstddef>
#include <iostream>

#define MEMORY_POOL_SIZE 1008 //536870904 // 512 MB - 8 Bytes

namespace APMemory
{
	struct MemControlBlock
	{
		bool IsAvailable;
		MemControlBlock* Next;
		MemControlBlock* Previous;

		inline std::size_t GetSize() const
		{
			// to get the bytes, multiply for sizeof
			// to get only the content of the block, subtract sizeof
			return (this->Next - this) * sizeof(MemControlBlock) - sizeof(MemControlBlock);
		}
	};

	void InitMemoryManager();
	void ShutdownMemoryManager();

	/** Uses Worst-Fit algorithm. */
	void* Alloc(const std::size_t BytesToAlloc);

	void Dealloc(void* SpaceToDealloc);

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
