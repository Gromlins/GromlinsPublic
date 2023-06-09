#include "utils.h"
#include "portable_executable.h"
#include <fstream>
#include <intrin.h>

namespace Utils
{
	void LogToFile(std::string file_name, std::string text)
	{
		std::ofstream myfile;

		myfile.seekp(std::ios_base::beg);

		myfile.open(file_name);
		myfile << text;
		myfile.close();
	}

	std::string ReadLineFromTxt(std::string file_name, int line_number)
	{
		std::string line;
		std::ifstream myfile(file_name);

		myfile.seekg(std::ios::beg);

		if (myfile.is_open())
		{
			for (int i = 0; i < line_number; ++i)
			{
				std::getline(myfile, line);
			}
			myfile.close();
		}

		return line;
	}

	void SetConsoleTextColor(int color_flags)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color_flags);
	}

	size_t SaveToFile(const char* path, uint8_t* buffer, size_t size)
	{
		auto file_handle = CreateFileA(
			path, GENERIC_ALL, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
		);

		SetFilePointer(file_handle, 0, 0, 0);

		uint32_t bytes;

		WriteFile(file_handle, buffer, size, (LPDWORD)&bytes, NULL);

		CloseHandle(file_handle);

		return size;
	}

	size_t LoadFileIntoMemory(const char* path, uint8_t** buffer)
	{
		auto file_handle = CreateFileA(
			path, GENERIC_ALL, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
		);

		std::cout << "GetLastError(): " << std::hex << GetLastError() << "\n";

		auto size = GetFileSize(file_handle, NULL);

		*buffer = (uint8_t*)VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		SetFilePointer(file_handle, 0, 0, 0);

		uint32_t bytes;

		ReadFile(file_handle, *buffer, size, (LPDWORD)&bytes, NULL);

		CloseHandle(file_handle);

		return size;
	}

	WORD GetProcId(const wchar_t* proc_name)
	{
		DWORD procId = 0;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (hSnap != INVALID_HANDLE_VALUE)
		{
			PROCESSENTRY32W procEntry;
			procEntry.dwSize = sizeof(procEntry);

			if (Process32FirstW(hSnap, &procEntry))
			{
				do
				{
					if (!_wcsicmp(procEntry.szExeFile, proc_name))
					{
						procId = procEntry.th32ProcessID;
						break;
					}
				} while (Process32NextW(hSnap, &procEntry));
			}
		}
		CloseHandle(hSnap);
		return procId;
	}

	bool IsAddressValid(PVOID address)
	{
		if (((uintptr_t)address < 0x7FFFFFFFFFFF) && ((uintptr_t)address > 0x1000))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
