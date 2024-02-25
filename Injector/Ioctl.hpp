#pragma once
#include <Windows.h>
#include <iostream>
#include "xorstr.hpp"
using namespace std;

#define DRIVER_NAME     (L"\\Driver\\lx91jznwop")
#define DRIVER_DEVICE_NAME     (L"\\Device\\lx91jznwop")
#define DRIVER_DOS_DEVICE_NAME (L"\\DosDevices\\lx91jznwop")
#define DRIVER_DEVICE_PATH xorstr_("\\\\.\\lx91jznwop")
#define DRIVER_DEVICE_TYPE 0x00000022

#define IOCTL_DRIVER_INIT ((ULONG)CTL_CODE(DRIVER_DEVICE_TYPE, 0xF101, METHOD_BUFFERED, FILE_ANY_ACCESS))
#define IOCTL_DRIVER_GET_BASE_ADDRESS ((ULONG)CTL_CODE(DRIVER_DEVICE_TYPE, 0xF111, METHOD_BUFFERED, FILE_ANY_ACCESS))
#define IOCTL_DRIVER_MANAGE_MEMORY ((ULONG)CTL_CODE(DRIVER_DEVICE_TYPE, 0xF122, METHOD_BUFFERED, FILE_ANY_ACCESS))
#define IOCTL_DRIVER_PROTECT ((ULONG)CTL_CODE(DRIVER_DEVICE_TYPE, 0xF143, METHOD_BUFFERED, FILE_ANY_ACCESS))

struct _initalize
{
	DWORD pid;
};

struct _baseaddress
{
	UINT64* out;
};

struct _memory
{
	UINT64 src, dst;
	SIZE_T size;
	BOOLEAN is_write, is_forcewrite;
};

struct _protect
{
	UINT64 addr;
	SIZE_T size;
	DWORD protect;
};