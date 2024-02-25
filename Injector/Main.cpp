#include "map.h"

#include <fstream>

#pragma warning(disable : 4996)

std::tuple<uint8_t*, size_t> GetFileBuffer(const char* buf)
{
	std::ifstream file(buf, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = new char[size];
	if (file.read(buffer, size))
	{
		return { (uint8_t*)buffer, size };
	}
}

int main(int argc, const char* argv[])
{
	auto [FileBuffer, FileSize] = GetFileBuffer(argv[1]);

	if (!FileBuffer || !FileSize)
	{
		return -1;
	}

	auto hhook = utils::LoadSignedHostDLL(driver->GetPIDByProcessName(xorstr_("TslGame.exe")), "C:\\libGLESv2.dll", &driver->rwx_module_base);

	Sleep(500); // wait til dll successfully loaded

	driver->Init(xorstr_("TslGame.exe"));

	driver->Protect(driver->rwx_module_base, FileSize + 0x2500);

	auto zero_data = new uint8_t[FileSize + 0x2500];
	ZeroMemory(zero_data, FileSize + 0x2500);

	driver->WPM(driver->rwx_module_base, zero_data, FileSize + 0x2500);
	
	map(FileBuffer);

	CloseHandle(driver->hDevice);

	system("pause");

	UnhookWindowsHookEx(hhook);

	return 0;
}