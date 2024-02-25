#pragma once
#include "Ioctl.hpp"
#include <TlHelp32.h>
#include <vector>

#pragma warning (disable : 4996)

class MemoryManagement
{
public:
	HANDLE hDevice;
	uint64_t ModuleBase;
	uint64_t rwx_module_base;
	bool Init(string ProcessName);
	template<typename T> T RPM(uint64_t lpBaseAddress);
	BOOL Read_Buf(uint64_t lpBaseAddress, uint64_t buf, size_t bufersize);
	template<typename T> void WPM(uint64_t lpBaseAddress, T Val);
	void WPM(uint64_t lpBaseAddress, void* Val, size_t size);
	uint32_t GetPIDByProcessName(string ProcessName);
	uint64_t GetImageBase();
	void Protect(uint64_t Address, size_t size, DWORD protect);
};

bool MemoryManagement::Init(string ProcessName)
{
	DWORD Bytes;
	this->hDevice = CreateFile(DRIVER_DEVICE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	if (this->hDevice == INVALID_HANDLE_VALUE)
		return 0;
	_initalize tmp;
	tmp.pid = this->GetPIDByProcessName(ProcessName);
	if (!tmp.pid)
		return false;

	DeviceIoControl(this->hDevice, IOCTL_DRIVER_INIT, &tmp, sizeof(tmp), &tmp, sizeof(tmp), &Bytes, 0);
	this->ModuleBase = this->GetImageBase();

	//rwx_module_base = GetModuleBase("lm.x64.dll");

	if (this->ModuleBase)
		return true;
	else
		return false;
}

BOOL MemoryManagement::Read_Buf(uint64_t lpBaseAddress, uint64_t buf, size_t bufersize)
{
	DWORD Bytes;
	_memory copy = {};
	copy.src = (ULONGLONG)buf;
	copy.dst = lpBaseAddress;
	copy.size = bufersize;
	copy.is_write = false;
	copy.is_forcewrite = false;

	if (DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, NULL))
		return TRUE;
	else
		return FALSE;
}

template<typename T> T MemoryManagement::RPM(uint64_t lpBaseAddress)
{
	DWORD Bytes;
	T Ret{};
	_memory copy = {};
	copy.src = (ULONGLONG)&Ret;
	copy.dst = lpBaseAddress;
	copy.size = sizeof(Ret);
	copy.is_write = false;
	copy.is_forcewrite = false;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, 0);
	return Ret;
}

template<typename T> void MemoryManagement::WPM(uint64_t lpBaseAddress, T Val)
{
	DWORD Bytes;
	T Ret;
	Ret = Val;
	_memory copy = {};
	copy.src = (ULONGLONG)&Ret;
	copy.dst = lpBaseAddress;
	copy.size = sizeof(Ret);
	copy.is_write = true;
	copy.is_forcewrite = false;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, 0);
	return;
}

void MemoryManagement::WPM(uint64_t lpBaseAddress, void* Val, size_t size)
{
	DWORD Bytes;
	_memory copy = {};
	copy.src = (ULONGLONG)Val;
	copy.dst = lpBaseAddress;
	copy.size = size;
	copy.is_write = true;
	copy.is_forcewrite = false;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_MANAGE_MEMORY, &copy, sizeof(copy), &copy, sizeof(copy), &Bytes, 0);
	return;
}


uint32_t MemoryManagement::GetPIDByProcessName(string ProcessName)
{
	DWORD Result = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(ProcEntry);
	do
		if (!strcmp(ProcEntry.szExeFile, ProcessName.c_str()))
		{
			if (ProcEntry.cntThreads > 2)
				Result = ProcEntry.th32ProcessID;
		}
	while (Process32Next(hSnap, &ProcEntry));
	if (hSnap)
		CloseHandle(hSnap);
	return Result;
}

uint64_t MemoryManagement::GetImageBase()
{
	DWORD Bytes;
	_baseaddress ModuleBase;
	uint64_t ModulePtr = 0;
	ModuleBase.out = &ModulePtr;
	DeviceIoControl(this->hDevice, IOCTL_DRIVER_GET_BASE_ADDRESS, &ModuleBase, sizeof(ModuleBase), &ModuleBase, sizeof(ModuleBase), &Bytes, 0);
	return ModulePtr;
}

void MemoryManagement::Protect(uint64_t Address, size_t size, DWORD protect = PAGE_EXECUTE_READWRITE)
{
	DWORD Bytes;
	_protect data;

	uint64_t ModulePtr = 0;
	data.addr = Address;
	data.size = size;
	data.protect = protect;

	DeviceIoControl(this->hDevice, IOCTL_DRIVER_PROTECT, &data, sizeof(data), &data, sizeof(data), &Bytes, 0);
}

inline MemoryManagement* driver = new MemoryManagement();