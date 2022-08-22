// Alessandro Pegoraro, 2022

#pragma once

#include <cstdlib>

namespace APMemory
{
	struct MemControlBlock
	{
		// This motherfucker has a 7 bytes padding! Dunno how to reduce it without messing with performance...
		bool IsAvailable;
		MemControlBlock* Next;
		MemControlBlock* Previous;

		inline std::size_t GetSize() const
		{
			const char* const EndOfThisBlockOverhead = (char*)(this + 1);
			const char* const StartOfNextBlock = (char*)this->Next;

			return StartOfNextBlock - EndOfThisBlockOverhead;
		}
	};
}
