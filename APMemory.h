#pragma once

#include <cstddef>
#include <iostream>

//536870912 = 512 MB

namespace APMemory
{
	struct MemControlBlock
	{
		bool IsAvailable;
		MemControlBlock* Next;
		MemControlBlock* Previous;

		inline std::size_t GetSize() const
		{
			const char* const EndOfThisBlock = (char*) (this + 1);
			const char* const StartOfNextBlock = (char*) this->Next;

			return StartOfNextBlock - EndOfThisBlock;
		}
	};

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
