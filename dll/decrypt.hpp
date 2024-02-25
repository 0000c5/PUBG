#pragma once
#include <Windows.h>

#include "offset.hpp"
#include "types.hpp"
#include "safecall.hpp"

class c_decrypt
{
public:
	bool init(uint64_t base)
	{
		uint64_t p_decrypt = *(uint64_t*)(base + offset::decrypt);

		if (p_decrypt)
		{
			int32_t Tmp1Add = *(uint32_t*)(p_decrypt + 3);
			if (Tmp1Add) 
			{
				decrypt_key = Tmp1Add + p_decrypt + 7;
				unsigned char ShellcodeBuff[1024] = { NULL };
				ShellcodeBuff[0] = 0x90;
				ShellcodeBuff[1] = 0x90;
				memcpy(&ShellcodeBuff[0x2], (PVOID)p_decrypt, sizeof(ShellcodeBuff) - 2);
				ShellcodeBuff[2] = 0x48;
				ShellcodeBuff[3] = 0x8B;
				ShellcodeBuff[4] = 0xC1;
				ShellcodeBuff[5] = 0x90;
				ShellcodeBuff[6] = 0x90;
				ShellcodeBuff[7] = 0x90;
				ShellcodeBuff[8] = 0x90;
				decrypt_fn = L(VirtualAlloc)(NULL, sizeof(ShellcodeBuff) + 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				memcpy((LPVOID)decrypt_fn, (LPVOID)ShellcodeBuff, sizeof(ShellcodeBuff));

				return true;
			}
		}

		return false;
	}

	uint64_t xenuine(uint64_t value)
	{
		if (!value) return 0;

		return reinterpret_cast<uint64_t(*)(uint64_t, uint64_t)>(decrypt_fn)(decrypt_key, value);
	}

	uint32_t index(uint32_t value)
	{
		if (!value) return 0;

		return _rotr(value ^ 0x2F567622, 0x000B) ^ (_rotr(value ^ 0x2F567622, 0x000B) << 0x0010) ^ 0x74590804;
	}

private:
	uint64_t decrypt_key;
	void* decrypt_fn;
};

c_decrypt decrypt;