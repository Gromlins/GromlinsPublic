#include "protect.h"
#include "memoryapi.h"
#include <libloaderapi.h>


PVOID Capture::DetourFunc(BYTE* src, const BYTE* dst, const int len)
{
    BYTE* jmp = (BYTE*)malloc(len + 5); DWORD dwback;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &dwback);
    memcpy(jmp, src, len); jmp += len; jmp[0] = 0xE9;

    *(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5; src[0] = 0xE9;
    *(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;

    VirtualProtect(src, len, dwback, &dwback);
    return (jmp - len);
}

void Capture::Thread()
{
    auto ThreadStartAddr = [](HANDLE hThread) -> DWORD
    {
        lpQueryInfo ThreadInformation = (lpQueryInfo)GetProcAddress(GetModuleHandle(es("ntdll.dll")), es("NtQueryInformationThread"));

        DWORD StartAddress;
        ThreadInformation(hThread, 9, &StartAddress, sizeof(DWORD), NULL);

        return StartAddress;
    };

    HANDLE CurrentThread = (HANDLE)(LONG_PTR)-2;
    DWORD  StartAddress = ThreadStartAddr(CurrentThread);

    if (StartAddress != 0x7626B0E0) {
        if (!TerminateThread(CurrentThread, 0xC0C))
            exit(0);
    }
    else exit(0);
}

BOOL Capture::HookLdrInitializeThunk()
{
    HMODULE hModule = LoadLibraryA(es("ntdll.dll"));
    if (hModule && (PBYTE)GetProcAddress(hModule, reinterpret_cast<LPCSTR>(es("LdrInitializeThunk"))))
    {
        DetourFunc((PBYTE)GetProcAddress(hModule, es("LdrInitializeThunk")), (PBYTE)Capture::Thread, 5);
        return TRUE;
    }
    else return FALSE;
}

bool Capture::HideThread(HANDLE hThread)
{
    typedef NTSTATUS(NTAPI* tNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);
    tNtSetInformationThread _NtSetInformationThread = (tNtSetInformationThread)GetProcAddress(GetModuleHandle(es("ntdll.dll")), es("NtSetInformationThread"));

    if (_NtSetInformationThread == NULL)
        return false;

    NTSTATUS Status;

    if (hThread == NULL)
        Status = _NtSetInformationThread(GetCurrentThread(), 0x11, 0, 0);
    else
        Status = _NtSetInformationThread(hThread, 0x11, 0, 0);

    if (Status != 0x00000000)
        return false;
    else
        return true;
}