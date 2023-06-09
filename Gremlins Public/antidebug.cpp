#include <windows.h>
#include <stdio.h>
#include <thread>
#include <string>
//#include <winternl.h>

#include "xorstr.h"

#include "antidebug.h"

#include "lazy.h"

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

//typedef NTSTATUS(*CALL)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
typedef DWORD(WINAPI* TCsrGetProcessId)(VOID);

typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
typedef NTSTATUS(__stdcall* _SetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);
typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);

void antidbg::init()
{
	while (true)
	{
		hidethread();

		if (debugstring() || remotepresent() || contextthread() || attached_dbg() /*|| peb_dbg()*/ || handle_dbg() || window_dbg())
			exit(1337);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

int antidbg::debugstring()
{
	LI_FN(SetLastError).safe()(0);
	LI_FN(OutputDebugStringA).safe()(es("fuck dbg"));
	const auto last_error = LI_FN(GetLastError).safe()();

	return last_error != 0;
}

int antidbg::hidethread()
{
	unsigned long thread_hide_from_debugger = 0x11;

	const auto ntdll = LI_FN(LoadLibraryA).safe()(es("ntdll.dll"));

	if (ntdll == INVALID_HANDLE_VALUE || ntdll == NULL) { return false; }

	_NtQueryInformationProcess NtQueryInformationProcess = NULL;
	NtQueryInformationProcess = (_NtQueryInformationProcess)LI_FN(GetProcAddress).safe()(ntdll, es("NtQueryInformationProcess"));

	if (NtQueryInformationProcess == NULL) { return false; }

	(_NtSetInformationThread)(GetCurrentThread(), thread_hide_from_debugger, 0, 0, 0);

	return true;
}

int antidbg::remotepresent()
{
	int debugger_present = false;

	LI_FN(CheckRemoteDebuggerPresent).safe()(LI_FN(GetCurrentProcess).safe()(), &debugger_present);

	return debugger_present;
}

int antidbg::contextthread()
{
	int found = false;
	CONTEXT ctx = { 0 };
	void* h_thread = LI_FN(GetCurrentThread).safe()();

	ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	if (LI_FN(GetThreadContext).safe()(h_thread, &ctx))
	{
		if ((ctx.Dr0 != 0x00) || (ctx.Dr1 != 0x00) || (ctx.Dr2 != 0x00) || (ctx.Dr3 != 0x00) || (ctx.Dr6 != 0x00) || (ctx.Dr7 != 0x00))
		{
			found = true;
		}
	}

	return found;
}

bool antidbg::attached_dbg()
{
	if (IsDebuggerPresent())
		return true;
	else
		return false;
}

//bool antidbg::peb_dbg()
//{
//	HANDLE h = GetCurrentProcess();
//	PROCESS_BASIC_INFORMATION ProcessInformation;
//	ULONG lenght = 0;
//	HINSTANCE ntdll;
//	CALL GetProcessInformation;
//	ntdll = LoadLibraryA(es("Ntdll.dll"));
//
//	//resolve address of NtQueryInformationProcess in ntdll.dll
//	GetProcessInformation = (CALL)GetProcAddress(ntdll, es("NtQueryInformationProcess"));
//
//	//get _PEB object
//	(GetProcessInformation)(h, ProcessBasicInformation, &ProcessInformation, sizeof(ProcessInformation), &lenght);
//
//	//checking PEB.BeingDebugged
//	BYTE BeingDebugged = ProcessInformation.PebBaseAddress->BeingDebugged;
//
//	return BeingDebugged;
//}

bool antidbg::handle_dbg()
{
	HMODULE hNtdll = LoadLibraryA(es("ntdll.dll"));
	if (!hNtdll)
		return false;

	TCsrGetProcessId pfnCsrGetProcessId = (TCsrGetProcessId)GetProcAddress(hNtdll, es("CsrGetProcessId"));
	if (!pfnCsrGetProcessId)
		return false;

	HANDLE hCsr = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pfnCsrGetProcessId());
	if (hCsr != NULL)
	{
		CloseHandle(hCsr);
		return true;
	}
	else
		return false;
}

bool antidbg::window_dbg()
{
	if (FindWindowA(NULL, es("IDA: Quick start"))) { return true; }
	else if (FindWindowA(NULL, es("Memory Viewer"))) { return true; }
	else if (FindWindowA(NULL, es("Process List"))) { return true; }
	else if (FindWindowA(NULL, es("KsDumper"))) { return true; }
	else if (FindWindowA(NULL, es("AnyDesk"))) { return true; }
	else if (FindWindowA(NULL, es("HTTP Debugger"))) { return true; }
	else if (FindWindowA(NULL, es("OllyDbg"))) { return true; }
	else if (FindWindowA(NULL, es("The Wireshark Network Analyzer"))) { return true; }
	else if (FindWindowA(NULL, es("Progress Telerik Fiddler Web Debugger"))) { return true; }
	else if (FindWindowA(NULL, es("Progress Telerik Fiddler Classic"))) { return true; }
	else if (FindWindowA(NULL, es("Fiddler"))) { return true; }
	else if (FindWindowA(NULL, es("HTTP Debugger"))) { return true; }
	else if (FindWindowA(NULL, es("KsDumper"))) { return true; }
	else if (FindWindowA(NULL, es("KsDumperClient"))) { return true; }
	else if (FindWindowA(NULL, es("x64dbg"))) { return true; }
	else if (FindWindowA(NULL, es("dnSpy"))) { return true; }
	else if (FindWindowA(NULL, es("FolderChangesView"))) { return true; }
	else if (FindWindowA(NULL, es("FolderChangesView (32 bit)"))) { return true; }
	else if (FindWindowA(NULL, es("BinaryNinja"))) { return true; }
	else if (FindWindowA(NULL, es("HxD"))) { return true; }
	else if (FindWindowA(NULL, es("Ida"))) { return true; }
	else if (FindWindowA(NULL, es("Ida Pro"))) { return true; }
	else if (FindWindowA(NULL, es("Ida Freeware"))) { return true; }
	else if (FindWindowA(NULL, es("HTTP Debugger Pro"))) { return true; }
	else if (FindWindowA(NULL, es("Process Hacker"))) { return true; }
	else if (FindWindowA(NULL, es("Process Hacker 2"))) { return true; }
	else if (FindWindowA(NULL, es("OllyDbg"))) { return true; }
	else if (FindWindowA(NULL, es("Lunar Engine"))) { return true; }
	else if (FindWindowA(es("WinDbgFrameClass"), NULL)) { return true; }
	else if (FindWindowA(es("WdcWindow"), NULL)) { return true; }
	//else if (FindWindowA(es("TaskManagerWindow"), NULL)) { return true; }
}

typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation = 0,
	ProcessDebugPort = 7,
	ProcessWow64Information = 26,
	ProcessImageFileName = 27,
	ProcessBreakOnTermination = 29
} PROCESSINFOCLASS;

EXCEPTION_DISPOSITION ExceptionRoutine(
	PEXCEPTION_RECORD ExceptionRecord,
	PVOID             EstablisherFrame,
	PCONTEXT          ContextRecord,
	PVOID             DispatcherContext)
{
	if (EXCEPTION_INVALID_HANDLE == ExceptionRecord->ExceptionCode)
	{
		exit(1337);
	}
	return ExceptionContinueExecution;
}

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_DEBUGGED (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

PVOID GetPEB64()
{
	PVOID pPeb = 0;
#ifndef _WIN64
	// 1. There are two copies of PEB - PEB64 and PEB32 in WOW64 process
	// 2. PEB64 follows after PEB32
	// 3. This is true for versions lower than Windows 8, else __readfsdword returns address of real PEB64
	if (IsWin8OrHigher())
	{
		BOOL isWow64 = FALSE;
		typedef BOOL(WINAPI* pfnIsWow64Process)(HANDLE hProcess, PBOOL isWow64);
		pfnIsWow64Process fnIsWow64Process = (pfnIsWow64Process)
			GetProcAddress(GetModuleHandleA(es("Kernel32.dll")), es("IsWow64Process"));
		if (fnIsWow64Process(GetCurrentProcess(), &isWow64))
		{
			if (isWow64)
			{
				pPeb = (PVOID)__readfsdword(0x0C * sizeof(PVOID));
				pPeb = (PVOID)((PBYTE)pPeb + 0x1000);
			}
		}
	}
#endif
	return pPeb;
}


// Current PEB for 64bit and 32bit processes accordingly
PVOID GetPEB()
{
	return (PVOID)__readgsqword(0x0C * sizeof(PVOID));
}


PIMAGE_NT_HEADERS GetImageNtHeaders(PBYTE pImageBase)
{
	PIMAGE_DOS_HEADER pImageDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	return (PIMAGE_NT_HEADERS)(pImageBase + pImageDosHeader->e_lfanew);
}
PIMAGE_SECTION_HEADER FindRDataSection(PBYTE pImageBase)
{
	static const std::string rdata = es(".rdata");
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_SECTION_HEADER pImageSectionHeader = IMAGE_FIRST_SECTION(pImageNtHeaders);
	int n = 0;
	for (; n < pImageNtHeaders->FileHeader.NumberOfSections; ++n)
	{
		if (rdata == (char*)pImageSectionHeader[n].Name)
		{
			break;
		}
	}
	return &pImageSectionHeader[n];
}
#pragma warning(disable : 4996)
WORD GetVersionWord()
{
	OSVERSIONINFO verInfo = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&verInfo);
	return MAKEWORD(verInfo.dwMinorVersion, verInfo.dwMajorVersion);
}
BOOL IsWin8OrHigher() { return GetVersionWord() >= _WIN32_WINNT_WIN8; }
BOOL IsVistaOrHigher() { return GetVersionWord() >= _WIN32_WINNT_VISTA; }

int GetHeapFlagsOffset(bool x64)
{
	return x64 ?
		IsVistaOrHigher() ? 0x70 : 0x14 : //x64 offsets
		IsVistaOrHigher() ? 0x40 : 0x0C; //x86 offsets
}
int GetHeapForceFlagsOffset(bool x64)
{
	return x64 ?
		IsVistaOrHigher() ? 0x74 : 0x18 : //x64 offsets
		IsVistaOrHigher() ? 0x44 : 0x10; //x86 offsets
}

typedef NTSTATUS(NTAPI* pfnNtQueryInformationProcess)(
	_In_      HANDLE           ProcessHandle,
	_In_      UINT             ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
	);

DWORD CalcFuncCrc(PUCHAR funcBegin, PUCHAR funcEnd)
{
	DWORD crc = 0;
	for (; funcBegin < funcEnd; ++funcBegin)
	{
		crc += *funcBegin;
	}
	return crc;
}
#pragma auto_inline(off)
VOID DebuggeeFunction()
{
	int calc = 0;
	calc += 2;
	calc <<= 8;
	calc -= 3;
}
VOID DebuggeeFunctionEnd()
{
};
#pragma auto_inline(on)
DWORD g_origCrc = 0x2bd0;

typedef NTSTATUS(NTAPI* pfnNtSetInformationThread)(
	_In_ HANDLE ThreadHandle,
	_In_ ULONG  ThreadInformationClass,
	_In_ PVOID  ThreadInformation,
	_In_ ULONG  ThreadInformationLength
	);
const ULONG ThreadHideFromDebugger = 0x11;

const int g_doSmthExecutionTime = 1050;
void DoSmth()
{
	Sleep(1000);
}
#define JUNK_CODE_ONE        \
    __asm{push eax}            \
    __asm{xor eax, eax}        \
    __asm{setpo al}            \
    __asm{push edx}            \
    __asm{xor edx, eax}        \
    __asm{sal edx, 2}        \
    __asm{xchg eax, edx}    \
    __asm{pop edx}            \
    __asm{or eax, ecx}        \
    __asm{pop eax}

void antidbg::SetupMain()
{
	HMODULE hNtDll = LoadLibrary(TEXT(es("ntdll.dll")));
	pfnNtSetInformationThread NtSetInformationThread = (pfnNtSetInformationThread)
		GetProcAddress(hNtDll, es("NtSetInformationThread"));
	NTSTATUS status = NtSetInformationThread(GetCurrentThread(),
		ThreadHideFromDebugger, NULL, 0);

	DWORD OldProtect = 0;


	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;

	while (1)
	{


		SYSTEMTIME sysTimeStart;
		SYSTEMTIME sysTimeEnd;
		FILETIME timeStart, timeEnd;
		HANDLE hProcess = NULL;
		DEBUG_EVENT de;
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si, sizeof(STARTUPINFO));
		ZeroMemory(&de, sizeof(DEBUG_EVENT));

		GetStartupInfo(&si);

		// Create the copy of ourself
		CreateProcess(NULL, GetCommandLine(), NULL, NULL, FALSE,
			DEBUG_PROCESS, NULL, NULL, &si, &pi);

		// Continue execution
		ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);

		// Wait for an event
		WaitForDebugEvent(&de, INFINITE);
		GetSystemTime(&sysTimeStart);
		DoSmth();
		GetSystemTime(&sysTimeEnd);
		SystemTimeToFileTime(&sysTimeStart, &timeStart);
		SystemTimeToFileTime(&sysTimeEnd, &timeEnd);
		double timeExecution = (timeEnd.dwLowDateTime - timeStart.dwLowDateTime) / 10000.0;
		if (timeExecution < g_doSmthExecutionTime)
		{

			if (IsDebuggerPresent())
			{
				exit(1337);
			}
			BOOL isDebuggerPresent = FALSE;
			if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent))
			{
				if (isDebuggerPresent)
				{
					exit(1337);
				}
			}
			PVOID pPeb = GetPEB();
			PVOID pPeb64 = GetPEB64();
			DWORD offsetNtGlobalFlag = 0;
			offsetNtGlobalFlag = 0xBC;
			DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + offsetNtGlobalFlag);
			if (NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
			{
				exit(1337);
			}
			if (pPeb64)
			{
				DWORD NtGlobalFlagWow64 = *(PDWORD)((PBYTE)pPeb64 + 0xBC);
				if (NtGlobalFlagWow64 & NT_GLOBAL_FLAG_DEBUGGED)
				{
					exit(1337);
				}
			}
			PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
			PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);

			HANDLE hExecutable = INVALID_HANDLE_VALUE;
			HANDLE hExecutableMapping = NULL;
			PBYTE pMappedImageBase = NULL;
			/*		__try
					{
						PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
						PIMAGE_SECTION_HEADER pImageSectionHeader = FindRDataSection(pImageBase);
						TCHAR pszExecutablePath[MAX_PATH];
						DWORD dwPathLength = GetModuleFileName(NULL, pszExecutablePath, MAX_PATH);
						if (0 == dwPathLength) __leave;
						hExecutable = CreateFile(pszExecutablePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
						if (INVALID_HANDLE_VALUE == hExecutable) __leave;
						hExecutableMapping = CreateFileMapping(hExecutable, NULL, PAGE_READONLY, 0, 0, NULL);
						if (NULL == hExecutableMapping) __leave;
						pMappedImageBase = (PBYTE)MapViewOfFile(hExecutableMapping, FILE_MAP_READ, 0, 0,
							pImageSectionHeader->PointerToRawData + pImageSectionHeader->SizeOfRawData);
						if (NULL == pMappedImageBase) __leave;
						PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pMappedImageBase);
						PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pMappedImageBase
							+ (pImageSectionHeader->PointerToRawData
								+ (pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress - pImageSectionHeader->VirtualAddress)));
						if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
						{
							std::cout << "Stop debugging program! 8" << std::endl;
							exit(1337);
						}
					}
					__finally
					{
						if (NULL != pMappedImageBase)
							UnmapViewOfFile(pMappedImageBase);
						if (NULL != hExecutableMapping)
							CloseHandle(hExecutableMapping);
						if (INVALID_HANDLE_VALUE != hExecutable)
							CloseHandle(hExecutable);
					}*/
			PVOID tpPeb = GetPEB();
			PVOID tpPeb64 = GetPEB64();
			PVOID heap = 0;
			DWORD offsetProcessHeap = 0;
			PDWORD heapFlagsPtr = 0, heapForceFlagsPtr = 0;
			BOOL x64 = FALSE;
			x64 = TRUE;
			offsetProcessHeap = 0x30;

			heap = (PVOID) * (PDWORD_PTR)((PBYTE)tpPeb + offsetProcessHeap);
			heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(x64));
			heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(x64));
			if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
			{
				exit(1337);
			}
			if (tpPeb64)
			{
				heap = (PVOID) * (PDWORD_PTR)((PBYTE)tpPeb64 + 0x30);
				heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(true));
				heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(true));
				if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
				{
					exit(1337);
				}
			}
			BOOL ttisDebuggerPresent = FALSE;
			if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &ttisDebuggerPresent))
			{
				if (ttisDebuggerPresent)
				{
					exit(1337);
				}
			}
			pfnNtQueryInformationProcess NtQueryInformationProcess = NULL;
			NTSTATUS status;
			DWORD tisDebuggerPresent = 0;
			HMODULE hNtDll = LoadLibrary(TEXT(es("ntdll.dll")));

			if (NULL != hNtDll)
			{
				NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(hNtDll, es("NtQueryInformationProcess"));
				if (NULL != NtQueryInformationProcess)
				{
					status = NtQueryInformationProcess(
						GetCurrentProcess(),
						ProcessDebugPort,
						&tisDebuggerPresent,
						sizeof(DWORD),
						NULL);
					if (status == 0x00000000 && tisDebuggerPresent != 0)
					{
						exit(1337);
					}
				}
			}
			SYSTEMTIME sysTimeStart;
			SYSTEMTIME sysTimeEnd;
			FILETIME timeStart, timeEnd;
			LPWSTR ProcessId = (LPWSTR)GetCurrentProcessId();
			if (CreateProcess(es("Antalco.exe"), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
			{

				GetSystemTime(&sysTimeStart);
				DoSmth();
				GetSystemTime(&sysTimeEnd);
				SystemTimeToFileTime(&sysTimeStart, &timeStart);
				SystemTimeToFileTime(&sysTimeEnd, &timeEnd);
				double timeExecution = (timeEnd.dwLowDateTime - timeStart.dwLowDateTime) / 10000.0;
				if (timeExecution < g_doSmthExecutionTime)
				{

					WaitForSingleObject(processInfo.hProcess, INFINITE);
					CloseHandle(processInfo.hProcess);
					CloseHandle(processInfo.hThread);
				}
				else
				{
					exit(1337);
				}
			}
		}
		else
		{
			exit(1337);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

}



void antidbg::roild() {

	SYSTEMTIME sysTimeStart;
	SYSTEMTIME sysTimeEnd;
	FILETIME timeStart, timeEnd;
	HMODULE hNtDll = LoadLibrary(TEXT(es("ntdll.dll")));
	pfnNtSetInformationThread NtSetInformationThread = (pfnNtSetInformationThread)
		GetProcAddress(hNtDll, es("NtSetInformationThread"));
	NTSTATUS status = NtSetInformationThread(GetCurrentThread(),
		ThreadHideFromDebugger, NULL, 0);

	if (IsDebuggerPresent())
	{
		exit(1337);
	}
	BOOL tisDebuggerPresent = FALSE;
	if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &tisDebuggerPresent))
	{
		if (tisDebuggerPresent)
		{
			exit(1337);
		}
	}
	PVOID pPeb = GetPEB();
	PVOID pPeb64 = GetPEB64();
	DWORD offsetNtGlobalFlag = 0;
	offsetNtGlobalFlag = 0xBC;
	DWORD NtGlobalFlag = *(PDWORD)((PBYTE)pPeb + offsetNtGlobalFlag);
	if (NtGlobalFlag & NT_GLOBAL_FLAG_DEBUGGED)
	{
		exit(1337);
	}
	if (pPeb64)
	{
		DWORD NtGlobalFlagWow64 = *(PDWORD)((PBYTE)pPeb64 + 0xBC);
		if (NtGlobalFlagWow64 & NT_GLOBAL_FLAG_DEBUGGED)
		{
			exit(1337);
		}
	}
	PBYTE pImageBase = (PBYTE)GetModuleHandle(NULL);
	PIMAGE_NT_HEADERS pImageNtHeaders = GetImageNtHeaders(pImageBase);
	PIMAGE_LOAD_CONFIG_DIRECTORY pImageLoadConfigDirectory = (PIMAGE_LOAD_CONFIG_DIRECTORY)(pImageBase
		+ pImageNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress);
	if (pImageLoadConfigDirectory->GlobalFlagsClear != 0)
	{
		exit(1337);
	}
	HANDLE hExecutable = INVALID_HANDLE_VALUE;
	HANDLE hExecutableMapping = NULL;
	PBYTE pMappedImageBase = NULL;

	PVOID tpPeb = GetPEB();
	PVOID tpPeb64 = GetPEB64();
	PVOID heap = 0;
	DWORD offsetProcessHeap = 0;
	PDWORD heapFlagsPtr = 0, heapForceFlagsPtr = 0;
	BOOL x64 = FALSE;
	x64 = TRUE;
	offsetProcessHeap = 0x30;
	heap = (PVOID) * (PDWORD_PTR)((PBYTE)tpPeb + offsetProcessHeap);
	heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(x64));
	heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(x64));
	if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
	{
		exit(1337);
	}
	if (tpPeb64)
	{
		heap = (PVOID) * (PDWORD_PTR)((PBYTE)tpPeb64 + 0x30);
		heapFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapFlagsOffset(true));
		heapForceFlagsPtr = (PDWORD)((PBYTE)heap + GetHeapForceFlagsOffset(true));
		if (*heapFlagsPtr & ~HEAP_GROWABLE || *heapForceFlagsPtr != 0)
		{
			exit(1337);
		}
	}
	BOOL ttisDebuggerPresent = FALSE;
	if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &ttisDebuggerPresent))
	{
		if (ttisDebuggerPresent)
		{
			exit(1337);
		}
	}
	pfnNtQueryInformationProcess NtQueryInformationProcess = NULL;
	NTSTATUS ttstatus;
	DWORD tttisDebuggerPresent = 0;
	HMODULE thNtDll = LoadLibrary(TEXT(es("ntdll.dll")));

	//SetupMain();

	if (NULL != thNtDll)
	{
		NtQueryInformationProcess = (pfnNtQueryInformationProcess)GetProcAddress(thNtDll, es("NtQueryInformationProcess"));
		if (NULL != NtQueryInformationProcess)
		{
			ttstatus = NtQueryInformationProcess(
				GetCurrentProcess(),
				ProcessDebugPort,
				&tttisDebuggerPresent,
				sizeof(DWORD),
				NULL);
			if (ttstatus == 0x00000000 && tttisDebuggerPresent != 0)
			{
				exit(1337);
			}
		}

	}
}