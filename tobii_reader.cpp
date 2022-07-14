#include "tobii_reader.h"

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

DWORD GetProcId(const wchar_t* procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                if (!_wcsicmp(procEntry.szExeFile, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

DWORD_PTR GetCoordinateBase() {
    DWORD_PTR firstOffset;
    DWORD_PTR secondOffset;

    DWORD procId = GetProcId(L"Tobii.EyeX.Engine.exe");

    if (procId == NULL) {
        throw exception("Process Not Found\n");
    }

    DWORD_PTR baseAddress = GetModuleBaseAddress(procId, L"Tobii.EyeX.Common.WindowsCache.dll") + 0x00048708;

    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

    ReadProcessMemory(procHandle, (LPCVOID)baseAddress, &firstOffset, sizeof(firstOffset), NULL);
    ReadProcessMemory(procHandle, (LPCVOID)(firstOffset + 0x60), &secondOffset, sizeof(secondOffset), 0);

    CloseHandle(procHandle);
    return secondOffset;
}

int GetX() {
    int x_pos;

    DWORD procId = GetProcId(L"Tobii.EyeX.Engine.exe");
    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

    ReadProcessMemory(procHandle, (LPCVOID)(GetCoordinateBase() + 0x9C), &x_pos, sizeof(x_pos), NULL);

    CloseHandle(procHandle);
    return x_pos;
}

int GetY() {
    int y_pos;

    DWORD procId = GetProcId(L"Tobii.EyeX.Engine.exe");
    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

    ReadProcessMemory(procHandle, (LPCVOID)(GetCoordinateBase() + 0xA0), &y_pos, sizeof(y_pos), NULL);

    CloseHandle(procHandle);
    return y_pos;
}