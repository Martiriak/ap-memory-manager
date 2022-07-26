#pragma once

#include <cstdlib>

namespace APMemory
{
	struct MemControlBlock
	{
		bool IsAvailable;
		MemControlBlock* Next;
		MemControlBlock* Previous;

		inline std::size_t GetSize() const
		{
			const char* const EndOfThisBlock = (char*)(this + 1);
			const char* const StartOfNextBlock = (char*)this->Next;

			return StartOfNextBlock - EndOfThisBlock;
		}
	};
}
