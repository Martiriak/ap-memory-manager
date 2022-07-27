#pragma once

enum class AllocatorType : char;
struct MemControlBlock;

namespace APMemory
{
	struct MemManagerInfo
	{
		/** The first block of all the allocated memory. */
		MemControlBlock* FirstBlock = nullptr;
		/** Intended as a guard, it is always marked as not available. */
		MemControlBlock* LastBlock = nullptr;
	};
}

