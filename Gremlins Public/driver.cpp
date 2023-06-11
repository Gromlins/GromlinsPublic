#include "driver.h"
#include "driver_bytes.h"
#include "portable_executable.h"

extern "C" int MapArray(uint8_t * array);

CMemory* mem = new CMemory;

HMODULE win32u{};
uint64_t BaseAddress{};
uintptr_t ProcessId{};

HANDLE driver_handle;

void* hooked_func;

void CMemory::DriverInitialize()
{
	mem->DriverMap(comm_driver);
	driver_handle = CreateFileW(L"\\\\.\\Gremlins_Device", GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

}

uint32_t CMemory::GetPIDByProcessName(const wchar_t* wszProcessName)
{
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32W pe = { sizeof(pe) };
	BOOL bOk = ::Process32FirstW(hSnapShot, &pe);
	while (bOk) {
		if (wcsstr(pe.szExeFile, wszProcessName) != NULL) {
			return pe.th32ProcessID;
		}

		bOk = ::Process32NextW(hSnapShot, &pe);
	}

	::CloseHandle(hSnapShot);

	return FALSE;
}

void CMemory::DriverMap(std::vector<uint8_t>& drv_buffer)
{
	MapArray(drv_buffer.data());
}

DWORD64 CMemory::GetModuleBase(const char* module_name)
{
	GetModuleMsg msg;

	uintptr_t result;

	msg.command_key = COMMAND_KEY;
	msg.message_id = module_base;

	int module_name_len = strlen(module_name);

	std::wstring module_wname(&module_name[0], &module_name[module_name_len]);

	wcscpy(msg.module, module_wname.c_str());
	msg.proc_id = ProcessId;

	uint32_t bytes;

	DeviceIoControl(driver_handle,
		COMMAND_KEY, &msg, sizeof(msg), &result, 8, (LPDWORD)&bytes, 0);

	return result;
}

void CMemory::ReadMemory(UINT_PTR read_address, void* buffer, size_t size)
{
	ReadCmd msg;

	msg.command_key = COMMAND_KEY;
	msg.message_id = read_mem;
	msg.proc_id = ProcessId;
	msg.address = read_address;
	msg.buffer = (uint8_t*)buffer;
	msg.size = size;

	uint32_t bytes;

	DeviceIoControl(driver_handle, COMMAND_KEY,
		&msg, sizeof(msg), 0, 0, (LPDWORD)&bytes, 0);

	return;
}

BOOL CMemory::WriteMemory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size)
{
	WriteCmd msg;

	msg.command_key = COMMAND_KEY;
	msg.message_id = write_mem;
	msg.proc_id = ProcessId;
	msg.address = write_address;
	msg.buffer = (uint8_t*)source_address;
	msg.size = write_size;

	uint32_t bytes;

	return DeviceIoControl(driver_handle, COMMAND_KEY,
		&msg, sizeof(msg), 0, 0, (LPDWORD)&bytes, 0);

	return true;
}

std::string CMemory::ReadString(UINT_PTR address)
{
	ReadCmd instructions;

	std::vector<char> buffer(sizeof(std::string), char{ 0 });

	instructions.message_id = DRIVER_CMD::read_mem;
	instructions.proc_id = ProcessId;
	instructions.size = buffer.size();
	instructions.address = address;
	instructions.buffer = static_cast<uint8_t*>((void*)&buffer[0]);

	uint32_t bytes;

	DeviceIoControl(driver_handle, COMMAND_KEY,
		&instructions, sizeof(instructions), 0, 0, (LPDWORD)&bytes, 0);

	return std::string(buffer.data());
}