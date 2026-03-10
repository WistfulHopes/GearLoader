#pragma once

#include <cstdint>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

#define ThreadQuerySetWin32StartAddress 9
typedef LONG (WINAPI *NtQueryInformationThread_t)(HANDLE, ULONG, PVOID, ULONG, PULONG);

DWORD GetMainThread(HANDLE& hThread) {
    constexpr intptr_t entryOffset = 0xC18A4;

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll) {
        return 1;
    }
    NtQueryInformationThread_t NtQueryInformationThread =
        reinterpret_cast<NtQueryInformationThread_t>(GetProcAddress(ntdll, "NtQueryInformationThread"));
    if (!NtQueryInformationThread) {
        return 2;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 3;
    DWORD pid = GetCurrentProcessId();

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    void* baseAddress = GetModuleHandle(NULL);
    void* mainThreadEntryPoint = reinterpret_cast<void*>(
        reinterpret_cast<intptr_t>(baseAddress) + entryOffset
    );

    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID != pid) continue;
            hThread = OpenThread(
                THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT,
                FALSE,
                threadEntry.th32ThreadID
            );

            if (hThread) {
                void* startAddr = nullptr;
                LONG status = NtQueryInformationThread(
                    hThread,
                    ThreadQuerySetWin32StartAddress,
                    &startAddr,
                    sizeof(startAddr),
                    NULL
                );
                if (startAddr == mainThreadEntryPoint) {
                    CloseHandle(snapshot);
                    return 0;
                }
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }
    CloseHandle(snapshot);
    return 4;
}

DWORD FindAndPauseMainThread(HANDLE& threadHandle) {
    DWORD result = GetMainThread(threadHandle);
    if (result !=0 ) {
        CloseHandle(threadHandle);
        return (result << 8) + 1;
    }
    result = SuspendThread(threadHandle);
    if (result != 0) {
        CloseHandle(threadHandle);
        return 2;
    }

    return 0;
}
