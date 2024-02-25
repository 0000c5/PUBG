#pragma once
#include <Windows.h>
#include <future>

#include "types.hpp"

const static uint64_t base = *(uint64_t*)(__readgsqword(0x60) + 0x10);

inline bool valid_ptr(uint64_t ptr)
{
	return (ptr > 0x2000000 && ptr < base + 0xFFFFFFFF);
}

template<typename T>
T read(uint64_t ptr)
{
	__try
	{
		T ret{};
		if (valid_ptr(ptr) && ReadProcessMemory(GetCurrentProcess(), (PVOID)ptr, &ret, sizeof(T), nullptr))
			return ret;
	}
	__except (1) {}
	return {};
}

void create_thread(void (*fptr)())
{
	static int count = 0;
	static std::future<void> list[50];
	list[count] = std::async(std::launch::async, fptr);
	count++;
}