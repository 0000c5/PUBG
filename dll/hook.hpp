#pragma once
#include "types.hpp"
#include "memory.hpp"

#include <vector>

namespace hook 
{
	void vmt(void* addr, void* pDes, int index, void** ret) 
	{
		auto vtable = *(uint64_t**)addr;
		int methods = 0;
		do {
			++methods;
		} while (*(uint64_t*)((uint64_t)vtable + (methods * 0x8)));
		auto vtable_buf = new uint64_t[methods];
		for (auto count = 0; count < methods; ++count) {
			vtable_buf[count] = *(uint64_t*)((uint64_t)vtable + (count * 0x8));

			*ret = (void*)vtable[index];

			vtable_buf[index] = (uint64_t)(pDes);
			*(uint64_t**)addr = vtable_buf;
		}
	}
}