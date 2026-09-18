#pragma once
template <class t>
t ReadMemory(uintptr_t address) {
	t read;
	ReadProcessMemory(Game.hProcess, (LPVOID)address, &read, sizeof(t), NULL);
	return read;
}

template <class T>
void WriteMemory(uintptr_t address, T value) {
	WriteProcessMemory(Game.hProcess, (LPVOID)address, &value, sizeof(T), NULL);
}
 
uintptr_t GetBaseAddress(const std::string& moduleName) {
     if (Game.pID == 0) {
        return 0;
    }

     HANDLE moduleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Game.pID);
    if (moduleSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(moduleEntry);

    if (!Module32First(moduleSnapshot, &moduleEntry)) {
        CloseHandle(moduleSnapshot); 
        return 0;
    }

    do {
        std::string currentModuleName(moduleEntry.szModule);

        if (currentModuleName.compare(moduleName) == 0) {
            uintptr_t baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
            CloseHandle(moduleSnapshot);
            return baseAddress;
        }
    } while (Module32Next(moduleSnapshot, &moduleEntry));

    CloseHandle(moduleSnapshot);
    return 0;
}

typedef NTSTATUS(NTAPI* tNtWriteVirtualMemory)(HANDLE, PVOID, PVOID, ULONG, PULONG);
tNtWriteVirtualMemory oNtWriteVirtualMemory = nullptr;

void WriteBytesNt(uintptr_t address, uint8_t* patch, size_t size) {
    ULONG bytesWritten = 0;
    if (!oNtWriteVirtualMemory) {
        HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
        if (hNtdll) {
            oNtWriteVirtualMemory = (tNtWriteVirtualMemory)GetProcAddress(hNtdll, "NtWriteVirtualMemory");
        }
    }

    if (oNtWriteVirtualMemory) {
        oNtWriteVirtualMemory(Game.hProcess, reinterpret_cast<PVOID>(address), patch, static_cast<ULONG>(size), &bytesWritten);
    }
}

void WriteBytes(uintptr_t address, uint8_t* patch, size_t size) {
    if (!oNtWriteVirtualMemory) {
        WriteBytesNt(address, patch, size);
    }
    else {
        SIZE_T bytesWritten;
        oNtWriteVirtualMemory(Game.hProcess, reinterpret_cast<PVOID>(address), patch, static_cast<ULONG>(size), (PULONG)&bytesWritten);
    }
}

std::string GetProcessNameByPID(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == nullptr) {
        return "";
    }

    char processName[MAX_PATH];
    DWORD dwSize = MAX_PATH;
    if (QueryFullProcessImageNameA(hProcess, 0, processName, &dwSize)) {
        std::string fullPath(processName);
        size_t pos = fullPath.find_last_of("\\/");
        if (pos != std::string::npos) {
            fullPath = fullPath.substr(pos + 1);
        }
        CloseHandle(hProcess);
        return fullPath;
    }
    CloseHandle(hProcess);
    return "";
}

uintptr_t GetBaseAddress() {
    if (Game.pID == 0) {
        return 0;
    }

    HANDLE moduleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Game.pID);
    if (moduleSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(moduleEntry);
    if (!Module32First(moduleSnapshot, &moduleEntry)) {
        CloseHandle(moduleSnapshot);
        return 0;
    }

    do {

        std::string currentModuleName(moduleEntry.szModule);
        if (currentModuleName.compare(GetProcessNameByPID(Game.pID)) == 0) {
            uintptr_t baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
            CloseHandle(moduleSnapshot);
            return baseAddress;
        }
    } while (Module32Next(moduleSnapshot, &moduleEntry));
    CloseHandle(moduleSnapshot);
    return 0;
}


/*Read String*/
  /*Rwc m*/
#include <Windows.h>
#include <winternl.h>
#include <string>

extern "C" NTSYSCALLAPI NTSTATUS ZwReadVirtualMemory(
    HANDLE hProcess,
    LPCVOID lpBaseAddress,
    LPVOID lpBuffer,
    SIZE_T nSize,
    SIZE_T* lpNumberOfBytesRead
);
std::string ReadString(uintptr_t Addr) {
    const int bufferSize = 256;
    char buffer[bufferSize];
    int bytesRead = 0;
    bool success = true;

    while (bytesRead < bufferSize) {
        char character;
        ZwReadVirtualMemory(Game.hProcess, (LPVOID)(Addr + bytesRead), &character, sizeof(char), NULL);
        buffer[bytesRead] = character;
        if (character == '\0') { break; }
        bytesRead++;
    }

    if (bytesRead == bufferSize) {
        success = false;
    }

    if (!success) {
        return "";
    }
    return std::string(buffer);
}
