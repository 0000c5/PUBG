#pragma once
#include <Windows.h>
#include <iostream>

#include "Memory.hpp"
#include "utils.h"

unsigned char remote_call_dll_main[136] = {
	0x48, 0x83, 0xEC, 0x38,
	0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x48, 0x39, 0xFF,
	0x90,
	0x39, 0xC0,
	0x90,
	0x48, 0x89, 0x44, 0x24, 0x20,
	0x90,
	0x48, 0x8B, 0x44, 0x24, 0x20,
	0x90,
	0x83, 0x38, 0x00,
	0x90,
	0x75, 0x48, // or 0x55
	0x48, 0x8B, 0x44, 0x24, 0x20,
	0x90, 0x90, 0x90,
	0xC7, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x90,
	0x48, 0x8B, 0x44, 0x24, 0x20,
	0x90,
	0x48, 0x8B, 0x40, 0x08,
	0x90,
	0x48, 0x89, 0x44, 0x24, 0x28,
	0x90,
	0x45, 0x33, 0xC0,
	0x90,
	0xBA, 0x01, 0x00, 0x00, 0x00,
	0x90, 0x90,
	0x48, 0x8B, 0x44, 0x24, 0x20,
	0x90,
	0x48, 0x8B, 0x48, 0x10,
	0x90, 0x90,
	0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x90,
	0x48, 0x8B, 0x44, 0x24, 0x20,
	0x90,
	0xC7, 0x00, 0xF1, 0x00, 0x00, 0x00,
	0x90,
	0x48, 0x83, 0xC4, 0x38,
	0x90,
	0xC3,
	0x90,
	0x48, 0x39, 0xC0,
	0x90,
	0xCC
};

typedef struct _remote_dll {
	INT status;
	uintptr_t dll_main_address;
	HINSTANCE dll_base;
} remote_dll, * premote_dll;

auto get_nt_headers(const uintptr_t image_base) -> IMAGE_NT_HEADERS*
{
	const auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*> (image_base);

	return reinterpret_cast<IMAGE_NT_HEADERS*> (image_base + dos_header->e_lfanew);
}

auto rva_va(const uintptr_t rva, IMAGE_NT_HEADERS* nt_header, void* local_image) -> void*
{
	const auto first_section = IMAGE_FIRST_SECTION(nt_header);

	for (auto section = first_section; section < first_section + nt_header->FileHeader.NumberOfSections; section++)
	{
		if (rva >= section->VirtualAddress && rva < section->VirtualAddress + section->Misc.VirtualSize)
		{
			return (unsigned char*)local_image + section->PointerToRawData + (rva - section->VirtualAddress);
		}
	}

	return 0;
}

auto relocate_image(void* remote_image, void* local_image, IMAGE_NT_HEADERS* nt_header) -> bool
{
	typedef struct _RELOC_ENTRY
	{
		ULONG ToRVA;
		ULONG Size;
		struct
		{
			WORD Offset : 12;
			WORD Type : 4;
		} Item[1];
	} RELOC_ENTRY, * PRELOC_ENTRY;

	const auto delta_offset = (uintptr_t)remote_image - nt_header->OptionalHeader.ImageBase;

	if (!delta_offset)
	{
		return true;
	}

	else if (!(nt_header->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE))
	{
		return false;
	}

	auto relocation_entry = (RELOC_ENTRY*)rva_va(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, nt_header, local_image);
	const auto relocation_end = (uintptr_t)relocation_entry + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

	if (relocation_entry == nullptr)
	{
		return true;
	}

	while ((uintptr_t)relocation_entry < relocation_end && relocation_entry->Size)
	{
		auto records_count = (relocation_entry->Size - 8) >> 1;

		for (auto i = 0ul; i < records_count; i++)
		{
			WORD fixed_type = (relocation_entry->Item[i].Type);
			WORD shift_delta = (relocation_entry->Item[i].Offset) % 4096;

			if (fixed_type == IMAGE_REL_BASED_ABSOLUTE)
			{
				continue;
			}

			if (fixed_type == IMAGE_REL_BASED_HIGHLOW || fixed_type == IMAGE_REL_BASED_DIR64)
			{
				auto fixed_va = (uintptr_t)rva_va(relocation_entry->ToRVA, nt_header, local_image);

				if (!fixed_va)
				{
					fixed_va = (uintptr_t)local_image;
				}

				*(uintptr_t*)(fixed_va + shift_delta) += delta_offset;
			}
		}

		relocation_entry = (PRELOC_ENTRY)((LPBYTE)relocation_entry + relocation_entry->Size);
	}

	return true;
}

auto resolve_function_address(LPCSTR module_name, LPCSTR function_name) -> uintptr_t
{
	const auto handle = LoadLibraryExA(module_name, nullptr, DONT_RESOLVE_DLL_REFERENCES);

	const auto offset = (uintptr_t)GetProcAddress(handle, function_name) - (uintptr_t)handle;

	FreeLibrary(handle);

	return offset;
}

auto write_sections(void* module_base, void* local_image, IMAGE_NT_HEADERS* nt_header) -> void
{
	auto section = IMAGE_FIRST_SECTION(nt_header);

	for (WORD count = 0; count < nt_header->FileHeader.NumberOfSections; count++, section++)
	{
		driver->WPM((uintptr_t)((uintptr_t)module_base + section->VirtualAddress), (void*)((uintptr_t)local_image + section->PointerToRawData), section->SizeOfRawData);
	}
}

auto resolve_import(void* local_image, IMAGE_NT_HEADERS* nt_header) -> bool
{
	IMAGE_IMPORT_DESCRIPTOR* import_description = (IMAGE_IMPORT_DESCRIPTOR*)rva_va(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress, nt_header, local_image);

	if (!nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress || !nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
	{
		return true;
	}

	LPSTR module_name = NULL;

	while ((module_name = (LPSTR)rva_va(import_description->Name, nt_header, local_image)))
	{
		const auto base_image = (ULONGLONG)LoadLibrary(module_name);// call_remote_load_library(pid, allocation, module_name);

		if (!base_image)
		{
			return false;
		}

		//	printf("%s : %p\n", module_name, base_image);

		auto import_header_data = (IMAGE_THUNK_DATA*)rva_va(import_description->FirstThunk, nt_header, local_image);

		while (import_header_data->u1.AddressOfData)
		{
			if (import_header_data->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				import_header_data->u1.Function = base_image + resolve_function_address(module_name, (LPCSTR)(import_header_data->u1.Ordinal & 0xFFFF));
			}
			else
			{
				IMAGE_IMPORT_BY_NAME* ibn = (IMAGE_IMPORT_BY_NAME*)rva_va(import_header_data->u1.AddressOfData, nt_header, local_image);
				import_header_data->u1.Function = base_image + resolve_function_address(module_name, (LPCSTR)ibn->Name);
			}
			import_header_data++;
		}
		import_description++;
	}

	return true;
}

auto erase_discardable_section(void* module_base, IMAGE_NT_HEADERS* nt_header) -> void
{
	auto section = IMAGE_FIRST_SECTION(nt_header);

	for (WORD count = 0; count < nt_header->FileHeader.NumberOfSections; count++, section++)
	{
		if (section->SizeOfRawData == 0)
		{
			continue;
		}

		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
		{
			auto zero_memory = VirtualAlloc(0, section->SizeOfRawData, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			ZeroMemory(zero_memory, section->SizeOfRawData);

			driver->WPM((uintptr_t)((uintptr_t)module_base + section->VirtualAddress), zero_memory, section->SizeOfRawData);

			VirtualFree(zero_memory, 0, MEM_RELEASE);
		}
	}
}

auto dll_main(void* base, IMAGE_NT_HEADERS* nt_header, void* shellcode_allocation) -> bool
{
	HMODULE nt_dll = LoadLibraryA(xorstr_("ntdll.dll"));

	DWORD process_id;
	DWORD thread_id = GetWindowThreadProcessId(FindWindowA(xorstr_("UnrealWindow"), NULL), &process_id);

	const auto shellcode_size = sizeof(remote_call_dll_main) + sizeof(remote_dll);

	const auto local_allocation = VirtualAlloc(0, shellcode_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	auto remote = (remote_dll*)((uintptr_t)local_allocation + sizeof(remote_call_dll_main));

	remote->dll_base = (HINSTANCE)base;

	remote->dll_main_address = ((uintptr_t)base + nt_header->OptionalHeader.AddressOfEntryPoint);

	memcpy(local_allocation, &remote_call_dll_main, sizeof(remote_call_dll_main));

	const auto shellcode_data = (uintptr_t)shellcode_allocation + sizeof(remote_call_dll_main);

	memcpy((void*)((uintptr_t)local_allocation + 0x6), &shellcode_data, sizeof(uintptr_t));

	memcpy((void*)((uintptr_t)local_allocation + 102), &remote->dll_main_address, sizeof(uintptr_t));

	driver->WPM((uintptr_t)shellcode_allocation, local_allocation, shellcode_size);

	printf("shellcode_allocation : %p\n", shellcode_allocation);

	printf("dll_main : %p\n", remote->dll_main_address);

	HHOOK h_hook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)shellcode_allocation, (HINSTANCE)driver->rwx_module_base, thread_id);

	while (remote->status != 0xF1)
	{
		PostThreadMessageW(thread_id, 0x1253, 0, 0);
		driver->Read_Buf((uintptr_t)shellcode_data, (uint64_t)remote, sizeof(remote_dll));
		Sleep(1);
	}

	printf(xorstr_("successfully executed\n"));

	ZeroMemory(local_allocation, shellcode_size);
	driver->WPM((uintptr_t)shellcode_allocation, local_allocation, shellcode_size);

	VirtualFree(local_allocation, 0, MEM_RELEASE);

	UnhookWindowsHookEx(h_hook);

	return true;
}

auto map(void* buffer) -> bool
{
	const auto nt_header = get_nt_headers(reinterpret_cast<uintptr_t>(buffer));
	
	printf(xorstr_("nt_headers: 0x%llx\n"), nt_header);

	const auto base = (void*)(driver->rwx_module_base + 0x1500);
	const auto shellcode_base = (void*)(driver->rwx_module_base + 0x1000);

	if (!shellcode_base)
	{
		printf(xorstr_("invalid rwx section\n"));
		return false;
	}

	if (!relocate_image(base, buffer, nt_header))
	{
		return false;
	}

	printf(xorstr_("relocated image\n"));

	if (!resolve_import(buffer, nt_header))
	{
		return false;
	}

	printf(xorstr_("resolved imports\n"));

	write_sections(base, buffer, nt_header);

	printf(xorstr_("wrote sections\n"));

	if (!dll_main(base, nt_header, shellcode_base))
	{
		return false;
	}

	erase_discardable_section(base, nt_header);

	printf(xorstr_("erased discardable section\n"));

	VirtualFree(buffer, 0, MEM_RELEASE);

	return true;
}