#pragma once
#include "defs.h"
#include "crt.h"

namespace utils
{
	void* get_system_information(SYSTEM_INFORMATION_CLASS information_class)
	{
		unsigned long size = 32;
		char buffer[32];

		ZwQuerySystemInformation(information_class, buffer, size, &size);

		void* info = ExAllocatePoolZero(NonPagedPool, size, 7265746172);

		if (!info)
			return nullptr;

		if (!NT_SUCCESS(ZwQuerySystemInformation(information_class, info, size, &size)))
		{
			ExFreePool(info);
			return nullptr;
		}

		return info;
	}

	uintptr_t get_kernel_module(const char* name)
	{
		const auto to_lower = [](char* string) -> const char*
			{
				for (char* pointer = string; *pointer != '\0'; ++pointer)
				{
					*pointer = (char)(short)tolower(*pointer);
				}

				return string;
			};

		const PRTL_PROCESS_MODULES info = (PRTL_PROCESS_MODULES)get_system_information(SystemModuleInformation);

		if (!info)
			return NULL;

		for (size_t i = 0; i < info->NumberOfModules; ++i)
		{
			const auto& mod = info->Modules[i];

			if (crt::strcmp(to_lower_c((char*)mod.FullPathName + mod.OffsetToFileName), name) == 0 || crt::strcmp(to_lower_c((char*)mod.FullPathName), name) == 0)
			{
				const void* address = mod.ImageBase;
				ExFreePool(info);
				return (uintptr_t)address;
			}
		}

		ExFreePool(info);
		return NULL;
	}
}