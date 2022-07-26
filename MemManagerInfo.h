#pragma once

enum class AllocatorType : char;
struct MemControlBlock;

namespace APMemory
{
	struct MemManagerInfo
	{
		bool IsInitialized = false;
		AllocatorType AllocatorInUse;

		MemControlBlock* FirstBlock = nullptr;
		MemControlBlock* LastBlock = nullptr;
	};
}

