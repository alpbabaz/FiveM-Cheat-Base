#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <cwctype>
#include <wchar.h>
#include <shlobj.h>
#include <ImGui/imgui.h>

namespace Destruct {

    constexpr float duration = 3.5f;

    inline std::wstring GetAppData() {
        wchar_t buf[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, buf)))
            return std::wstring(buf);
        return L"";
    }

    
    inline void PerformCleanup() {
        std::vector<std::wstring> targets;
        std::wstring app = GetAppData();
        targets.push_back(app + L"\\alp-fy0w\\");
        targets.push_back(app + L"\\ObsText\\");
        targets.push_back(app + L"\\AuthServer\\");
        targets.push_back(app + L"\\CFXBASE\\");
        targets.push_back(L"C:\\alp_fy0w_debug.txt");
        targets.push_back(L"C:\\_alp_debug.txt");

        for (const std::wstring& t : targets) {
            DWORD attr = GetFileAttributesW(t.c_str());
            if (attr == INVALID_FILE_ATTRIBUTES) continue;
            if (attr & FILE_ATTRIBUTE_DIRECTORY)
                RemoveDirectoryW(t.c_str());
            else
                DeleteFileW(t.c_str());
        }

        
        std::wstring recent = app + L"\\Microsoft\\Windows\\Recent\\";
        WIN32_FIND_DATAW fd;
        HANDLE hFind = FindFirstFileW((recent + L"*").c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                std::wstring name = fd.cFileName;
                std::wstring lower = name;
                for (auto& c : lower) c = (wchar_t)towlower(c);
                if (lower.find(L"alp") != std::wstring::npos ||
                    lower.find(L"fy0w") != std::wstring::npos ||
                    lower.find(L"obs-text") != std::wstring::npos)
                    DeleteFileW((recent + name).c_str());
            } while (FindNextFileW(hFind, &fd));
            FindClose(hFind);
        }
    }

    
    inline void DrawScreen(float elapsed, const ImVec2& size, ImDrawList* fg) {
        if (!fg) return;
        float t = elapsed / duration;
        if (t < 0.f) t = 0.f;
        if (t > 1.f) t = 1.f;

        unsigned char alpha = (unsigned char)(t * 255.f);
        ImU32 col = IM_COL32(0, 0, 0, alpha);
        fg->AddRectFilled(ImVec2(0, 0), size, col);

        if (t < 1.f) {
            char buf[128];
            snprintf(buf, sizeof(buf), "Cleaning up %.0f%%", t * 100.f);
            float w = 11.0f * sizeof(buf);
            fg->AddText(nullptr, 22.f, ImVec2((size.x - w) * 0.5f, size.y * 0.4f), IM_COL32(255, 255, 255, 200), buf);
        }
    }

    
    inline void KillOBS() {
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return;
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(pe);
        if (Process32FirstW(snap, &pe)) {
            do {
                std::wstring name = pe.szExeFile;
                std::wstring lower = name;
                for (auto& ch : lower) ch = (wchar_t)towlower(ch);
                if (lower == L"obs64.exe" || lower == L"obs32.exe" || lower == L"obs.exe") {
                    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                    if (h) {
                        TerminateProcess(h, 0);
                        CloseHandle(h);
                    }
                }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
    }
}