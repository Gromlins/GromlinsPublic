#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <cstdint>
#include <TlHelp32.h>
#include <iostream>

namespace Utils
{
	bool IsHypervisorPresent();
	void SetConsoleTextColor(int color_flags);

	size_t LoadFileIntoMemory(const char* path, uint8_t** buffer);
	size_t LoadFileIntoMemory(const char* path, char** buffer);
	bool IsAddressValid(PVOID address);
	WORD GetProcId(const wchar_t* proc_name);

	/*	true for AMD, false for intel	*/
	bool IsIntelOrAmd();
	size_t SaveToFile(const char* path, uint8_t* buffer, size_t size);

	void LogToFile(std::string file_name, std::string text);

	std::string ReadLineFromTxt(std::string file_name, int line_number);
};