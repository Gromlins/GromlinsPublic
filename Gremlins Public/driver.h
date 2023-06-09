#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <Dwmapi.h>
#include <array>
#include <vector>
#include <string_view>
#include <memory>
#include <mutex>

#include "xorstr.h"

extern HMODULE win32u;
extern uint64_t BaseAddress;
extern uintptr_t ProcessId;

extern HANDLE driver_handle;

#define COMMAND_KEY 0xDEADBEEF

enum DRIVER_CMD : uint32_t
{
	none,
	write_mem,
	read_mem,
	module_base,
	remote_npt_hook,
	hide_memory,
	protect_memory
};

struct Msg
{
	int64_t command_key;
	int message_id;
};

struct GetProcessIdMsg : Msg
{
	wchar_t process_name[50];
};

struct NptHookMsg : Msg
{
	int32_t proc_id;
	size_t size;
	uintptr_t hook_address;
	uint8_t shellcode[0x1000];
};

struct AllocMemCmd : Msg
{
	int32_t proc_id;
	size_t size;
	wchar_t	section_name[60];
};

struct InvokeRemoteFunctionCmd : Msg
{
	int proc_id;
	uintptr_t map_base;
	uintptr_t address;
	uintptr_t image_size;
	uintptr_t RtlAddFunctionTable_address;
};

struct GetModuleMsg : Msg
{
	uint32_t proc_id;
	wchar_t module[50];
};

struct WriteCmd : Msg
{
	int proc_id;
	uintptr_t address;
	uint8_t* buffer;
	int size;
};

struct ReadCmd : Msg
{
	int proc_id;
	uintptr_t address;
	uint8_t* buffer;
	int size;
};

struct ProtectMemoryMsg : Msg
{
	int proc_id;
	uintptr_t address;
	uint32_t memory_protection;
	uint32_t size;
};

struct HideMemoryCmd : Msg
{
	int32_t target_pid;
	uintptr_t address;
	uintptr_t hiding_range_size;
};

extern "C"
{
	void InitDriver();
}


class CMemory
{
public:

	void DriverInitialize();

	void DriverMap(std::vector<uint8_t>& drv_buffer);

	uint32_t GetPIDByProcessName(const wchar_t* wszProcessName);

	DWORD64 GetModuleBase(const char* module_name);

	void ReadMemory(UINT_PTR read_address, void* buffer, size_t size);

	BOOL WriteMemory(UINT_PTR write_address, UINT_PTR source_address, SIZE_T write_size);

	std::string ReadString(UINT_PTR address);

	template<typename T> bool WPM(uint64_t Address, const T& _Value)
	{
		return WriteMemory(Address, (UINT_PTR)&_Value, sizeof(T));
	}
	template<typename T> bool WPM(uint32_t Address, const T& _Value)
	{
		return WriteMemory(__int64(Address), (UINT_PTR)&_Value, sizeof(T));
	}
	template<typename T> T RPM(uint64_t Address)
	{
		T _Value;
		ReadMemory(Address, &_Value, sizeof(T));
		return _Value;
	}
	template<typename T> T RPM(uint32_t Address)
	{
		T _Value;
		ReadMemory(__int64(Address), &_Value, sizeof(T));
		return _Value;
	}
};

extern CMemory* mem;
