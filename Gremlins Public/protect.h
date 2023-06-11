#pragma once
#include <string>
#include <Windows.h>
#include <iostream>
#include <Psapi.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <future>

#include "xorstr.h"

#pragma comment(lib, "psapi.lib")

typedef NTSTATUS(WINAPI* lpQueryInfo)(HANDLE, LONG, PVOID, ULONG, PULONG);

namespace Capture
{
	PVOID DetourFunc(BYTE* src, const BYTE* dst, const int len);
	void Thread();
	BOOL HookLdrInitializeThunk();
	bool HideThread(HANDLE hThread);
}

