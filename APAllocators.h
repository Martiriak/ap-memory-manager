// Alessandro Pegoraro, 2022

#pragma once

#include <cstddef>

namespace APMemory
{
	template<typename T>
	class APAllocator
	{
	public:

		typedef T value_type;

		APAllocator() noexcept {}
		template<class U> APAllocator(const APAllocator<U>&) noexcept {}

		inline T* allocate(std::size_t NumberOfObjects)
		{
			return static_cast<T*>(Alloc(NumberOfObjects * sizeof(T)));
		}

		inline void deallocate(T* ObjectToDealloc, std::size_t NumberOfObjects)
		{
			Dealloc(ObjectToDealloc, NumberOfObjects * sizeof(T));
		}
	};


	/**
	* Needed by our internal vectors in order for them to not rely
	* on global operators new and delete, which would provoke a
	* catastrophic loop if the macro APMEMORY_DISABLE_NEW_DELETE_OVERRIDE
	* has been defined.
	*/
	template<typename T>
	class CHeapAllocator
	{
	public:

		typedef T value_type;

		CHeapAllocator() noexcept {}
		template<class U> CHeapAllocator(const CHeapAllocator<U>&) noexcept {}

		inline T* allocate(std::size_t NumberOfObjects)
		{
			return static_cast<T*>(malloc(NumberOfObjects * sizeof(T)));
		}

		inline void deallocate(T* ObjectToDealloc, std::size_t NumberOfObjects)
		{
			free(ObjectToDealloc);
		}
	};

} // APMemory End
