#pragma once
#include <windows.h>
#include <string>
#include <vector>


class StringCleaner {
public:
    DWORD pid;
    std::vector<std::string> strings;

    StringCleaner(DWORD processId, const std::vector<std::string>& targets)
        : pid(processId), strings(targets) {}

    void clean(HANDLE hProcess) {
        if (!hProcess || strings.empty()) return;
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        MEMORY_BASIC_INFORMATION mbi;
        unsigned char* addr = (unsigned char*)si.lpMinimumApplicationAddress;
        while (addr < (unsigned char*)si.lpMaximumApplicationAddress) {
            if (VirtualQueryEx(hProcess, addr, &mbi, sizeof(mbi)) != sizeof(mbi)) break;
            if (mbi.State == MEM_COMMIT &&
                mbi.Protect != PAGE_NOACCESS &&
                (mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_WRITECOPY))) {
                SIZE_T region = mbi.RegionSize;
                std::vector<char> buf(region);
                SIZE_T read = 0;
                if (ReadProcessMemory(hProcess, addr, buf.data(), region, &read) && read > 0) {
                    std::string haystack(buf.data(), read);
                    for (const std::string& s : strings) {
                        if (s.empty()) continue;
                        size_t pos = 0;
                        while ((pos = haystack.find(s, pos)) != std::string::npos) {
                            WriteProcessMemory(hProcess, addr + pos, std::string(s.size(), '\0').data(), s.size(), nullptr);
                            pos += s.size();
                        }
                    }
                }
            }
            addr += mbi.RegionSize;
        }
    }
};