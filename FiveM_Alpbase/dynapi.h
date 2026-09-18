#pragma once
#include <Windows.h>

namespace DynAPI {
    
    typedef HANDLE(WINAPI* pOpenProcess)(DWORD, BOOL, DWORD);
    typedef BOOL(WINAPI* pReadProcessMemory)(HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T*);
    typedef BOOL(WINAPI* pWriteProcessMemory)(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T*);
    typedef BOOL(WINAPI* pVirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD);
    typedef SIZE_T(WINAPI* pVirtualQueryEx)(HANDLE, LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T);
    typedef HANDLE(WINAPI* pCreateToolhelp32Snapshot)(DWORD, DWORD);
    typedef BOOL(WINAPI* pProcess32FirstW)(HANDLE, LPPROCESSENTRY32W);
    typedef BOOL(WINAPI* pProcess32NextW)(HANDLE, LPPROCESSENTRY32W);
    typedef BOOL(WINAPI* pIsDebuggerPresent)();
    typedef BOOL(WINAPI* pCheckRemoteDebuggerPresent)(HANDLE, PBOOL);
    typedef BOOL(WINAPI* pGetThreadContext)(HANDLE, LPCONTEXT);
    typedef DWORD(WINAPI* pGetTickCount)();
    typedef ULONGLONG(WINAPI* pGetTickCount64)();
    typedef BOOL(WINAPI* pQueryPerformanceCounter)(PLARGE_INTEGER);
    typedef BOOL(WINAPI* pQueryPerformanceFrequency)(PLARGE_INTEGER);
    typedef int(WINAPI* pGetSystemMetrics)(int);
    typedef BOOL(WINAPI* pSetWindowDisplayAffinity)(HWND, DWORD);
    typedef DWORD(WINAPI* pGetFileAttributesW)(LPCWSTR);
    typedef BOOL(WINAPI* pDeleteFileW)(LPCWSTR);
    typedef BOOL(WINAPI* pSetFileAttributesW)(LPCWSTR, DWORD);
    typedef HANDLE(WINAPI* pFindFirstFileW)(LPCWSTR, LPWIN32_FIND_DATAW);
    typedef BOOL(WINAPI* pFindNextFileW)(HANDLE, LPWIN32_FIND_DATAW);
    typedef BOOL(WINAPI* pFindClose)(HANDLE);
    typedef DWORD(WINAPI* pGetTempPathW)(DWORD, LPWSTR);
    typedef HMODULE(WINAPI* pLoadLibraryW)(LPCWSTR);
    typedef HMODULE(WINAPI* pLoadLibraryA)(LPCWSTR);
    typedef FARPROC(WINAPI* pGetProcAddress)(HMODULE, LPCSTR);
    typedef HMODULE(WINAPI* pGetModuleHandleW)(LPCWSTR);
    typedef HMODULE(WINAPI* pGetModuleHandleA)(LPCSTR);
    typedef void(WINAPI* pGetSystemInfo)(LPSYSTEM_INFO);
    typedef BOOL(WINAPI* pCloseHandle)(HANDLE);
    typedef DWORD(WINAPI* pGetCurrentThreadId)();
    typedef HANDLE(WINAPI* pGetCurrentThread)();
    typedef HANDLE(WINAPI* pGetCurrentProcess)();
    typedef BOOL(WINAPI* pOpenProcessToken)(HANDLE, DWORD, PHANDLE);
    typedef BOOL(WINAPI* pLookupPrivilegeValueW)(LPCWSTR, LPCWSTR, PLUID);
    typedef BOOL(WINAPI* pAdjustTokenPrivileges)(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD);
    typedef BOOL(WINAPI* pGetAdaptersInfo)(void*, PULONG);
    typedef DWORD(WINAPI* pGetSystemFirmwareTable)(DWORD, DWORD, PVOID, DWORD);
    typedef BOOL(WINAPI* pGetUserNameW)(LPWSTR, LPDWORD);
    typedef int(WINAPI* pMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);

    
    typedef SHORT(WINAPI* pGetAsyncKeyState)(int);
    typedef HWND(WINAPI* pFindWindowW)(LPCWSTR, LPCWSTR);
    typedef HWND(WINAPI* pFindWindowA)(LPCSTR, LPCSTR);
    typedef BOOL(WINAPI* pGetClientRect)(HWND, LPRECT);
    typedef BOOL(WINAPI* pClientToScreen)(HWND, LPPOINT);
    typedef BOOL(WINAPI* pSetWindowPos)(HWND, HWND, int, int, int, int, UINT);
    typedef LONG(WINAPI* pSetWindowLongW)(HWND, int, LONG);
    typedef BOOL(WINAPI* pSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
    typedef BOOL(WINAPI* pUpdateWindow)(HWND);
    typedef BOOL(WINAPI* pShowWindow)(HWND, int);
    typedef BOOL(WINAPI* pDestroyWindow)(HWND);
    typedef BOOL(WINAPI* pPostMessageW)(HWND, UINT, WPARAM, LPARAM);
    typedef HDC(WINAPI* pGetDC)(HWND);
    typedef int(WINAPI* pReleaseDC)(HWND, HDC);
    typedef HDC(WINAPI* pCreateCompatibleDC)(HDC);
    typedef HBITMAP(WINAPI* pCreateCompatibleBitmap)(HDC, int, int);
    typedef HGDIOBJ(WINAPI* pSelectObject)(HDC, HGDIOBJ);
    typedef BOOL(WINAPI* pBitBlt)(HDC, int, int, int, int, HDC, int, int, DWORD);
    typedef BOOL(WINAPI* pStretchBlt)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
    typedef int(WINAPI* pSetStretchBltMode)(HDC, int);
    typedef BOOL(WINAPI* pDeleteObject)(HGDIOBJ);
    typedef BOOL(WINAPI* pDeleteDC)(HDC);
    typedef int(WINAPI* pGetDIBits)(HDC, HBITMAP, UINT, UINT, LPVOID, LPBITMAPINFO, UINT);
    typedef BOOL(WINAPI* pEnumDisplaySettingsW)(LPCWSTR, DWORD, DEVMODEW);
    typedef LONG(WINAPI* pChangeDisplaySettingsExW)(LPCWSTR, DEVMODEW, HWND, DWORD, LPVOID);
    typedef BOOL(WINAPI* pSetCursorPos)(int, int);
    typedef BOOL(WINAPI* pPeekMessageW)(LPMSG, HWND, UINT, UINT, UINT);
    typedef BOOL(WINAPI* pTranslateMessage)(const MSG*);
    typedef LRESULT(WINAPI* pDispatchMessageW)(const MSG*);
    typedef LRESULT(WINAPI* pDefWindowProcW)(HWND, UINT, WPARAM, LPARAM);
    typedef ATOM(WINAPI* pRegisterClassExW)(const WNDCLASSEXW*);
    typedef BOOL(WINAPI* pUnregisterClassW)(LPCWSTR, HINSTANCE);
    typedef HWND(WINAPI* pCreateWindowExW)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
    typedef BOOL(WINAPI* pSetLayeredWindowAttributes2)(HWND, COLORREF, BYTE, DWORD);

    
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, ULONG, PVOID, ULONG);
    typedef NTSTATUS(NTAPI* pNtQuerySystemInformation)(ULONG, PVOID, ULONG, PULONG);

    
    inline pOpenProcess              OpenProcess_ = nullptr;
    inline pReadProcessMemory        ReadProcessMemory_ = nullptr;
    inline pWriteProcessMemory       WriteProcessMemory_ = nullptr;
    inline pVirtualProtect           VirtualProtect_ = nullptr;
    inline pVirtualQueryEx           VirtualQueryEx_ = nullptr;
    inline pCreateToolhelp32Snapshot CreateToolhelp32Snapshot_ = nullptr;
    inline pProcess32FirstW          Process32FirstW_ = nullptr;
    inline pProcess32NextW           Process32NextW_ = nullptr;
    inline pIsDebuggerPresent        IsDebuggerPresent_ = nullptr;
    inline pCheckRemoteDebuggerPresent CheckRemoteDebuggerPresent_ = nullptr;
    inline pGetThreadContext         GetThreadContext_ = nullptr;
    inline pGetTickCount64           GetTickCount64_ = nullptr;
    inline pQueryPerformanceCounter  QueryPerformanceCounter_ = nullptr;
    inline pQueryPerformanceFrequency QueryPerformanceFrequency_ = nullptr;
    inline pGetSystemMetrics         GetSystemMetrics_ = nullptr;
    inline pSetWindowDisplayAffinity SetWindowDisplayAffinity_ = nullptr;
    inline pGetFileAttributesW       GetFileAttributesW_ = nullptr;
    inline pDeleteFileW              DeleteFileW_ = nullptr;
    inline pSetFileAttributesW       SetFileAttributesW_ = nullptr;
    inline pFindFirstFileW           FindFirstFileW_ = nullptr;
    inline pFindNextFileW            FindNextFileW_ = nullptr;
    inline pFindClose                FindClose_ = nullptr;
    inline pGetTempPathW             GetTempPathW_ = nullptr;
    inline pLoadLibraryW             LoadLibraryW_ = nullptr;
    inline pGetProcAddress           GetProcAddress_ = nullptr;
    inline pGetModuleHandleW         GetModuleHandleW_ = nullptr;
    inline pGetModuleHandleA         GetModuleHandleA_ = nullptr;
    inline pGetSystemInfo            GetSystemInfo_ = nullptr;
    inline pCloseHandle              CloseHandle_ = nullptr;
    inline pGetCurrentThreadId       GetCurrentThreadId_ = nullptr;
    inline pGetCurrentThread         GetCurrentThread_ = nullptr;
    inline pGetCurrentProcess        GetCurrentProcess_ = nullptr;
    inline pOpenProcessToken         OpenProcessToken_ = nullptr;
    inline pLookupPrivilegeValueW    LookupPrivilegeValueW_ = nullptr;
    inline pAdjustTokenPrivileges    AdjustTokenPrivileges_ = nullptr;
    inline pGetSystemFirmwareTable   GetSystemFirmwareTable_ = nullptr;
    inline pGetUserNameW             GetUserNameW_ = nullptr;
    inline pMessageBoxA              MessageBoxA_ = nullptr;

    inline pGetAsyncKeyState         GetAsyncKeyState_ = nullptr;
    inline pFindWindowW              FindWindowW_ = nullptr;
    inline pFindWindowA              FindWindowA_ = nullptr;
    inline pGetClientRect            GetClientRect_ = nullptr;
    inline pClientToScreen           ClientToScreen_ = nullptr;
    inline pSetWindowPos             SetWindowPos_ = nullptr;
    inline pSetWindowLongW           SetWindowLongW_ = nullptr;
    inline pUpdateWindow             UpdateWindow_ = nullptr;
    inline pShowWindow               ShowWindow_ = nullptr;
    inline pDestroyWindow            DestroyWindow_ = nullptr;
    inline pPostMessageW             PostMessageW_ = nullptr;
    inline pGetDC                    GetDC_ = nullptr;
    inline pReleaseDC                ReleaseDC_ = nullptr;
    inline pCreateCompatibleDC       CreateCompatibleDC_ = nullptr;
    inline pCreateCompatibleBitmap   CreateCompatibleBitmap_ = nullptr;
    inline pSelectObject             SelectObject_ = nullptr;
    inline pBitBlt                   BitBlt_ = nullptr;
    inline pStretchBlt               StretchBlt_ = nullptr;
    inline pSetStretchBltMode        SetStretchBltMode_ = nullptr;
    inline pDeleteObject             DeleteObject_ = nullptr;
    inline pDeleteDC                 DeleteDC_ = nullptr;
    inline pGetDIBits                GetDIBits_ = nullptr;
    inline pPeekMessageW             PeekMessageW_ = nullptr;
    inline pTranslateMessage         TranslateMessage_ = nullptr;
    inline pDispatchMessageW         DispatchMessageW_ = nullptr;
    inline pDefWindowProcW           DefWindowProcW_ = nullptr;
    inline pRegisterClassExW         RegisterClassExW_ = nullptr;
    inline pUnregisterClassW         UnregisterClassW_ = nullptr;
    inline pCreateWindowExW          CreateWindowExW_ = nullptr;

    inline pNtQueryInformationProcess NtQueryInformationProcess_ = nullptr;
    inline pNtSetInformationThread   NtSetInformationThread_ = nullptr;
    inline pNtQuerySystemInformation NtQuerySystemInformation_ = nullptr;

    inline bool initialized = false;

    inline void Init() {
        if (initialized) return;

        char k32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l','\0' };
        char u32[] = { 'u','s','e','r','3','2','.','d','l','l','\0' };
        char ntd[] = { 'n','t','d','l','l','.','d','l','l','\0' };
        char iph[] = { 'i','p','h','l','p','a','p','i','.','d','l','l','\0' };

        HMODULE hk32 = GetModuleHandleA(k32);
        HMODULE hu32 = GetModuleHandleA(u32);
        HMODULE hntd = GetModuleHandleA(ntd);
        HMODULE hiph = GetModuleHandleA(iph);

        if (!hk32) hk32 = LoadLibraryA(k32);
        if (!hu32) hu32 = LoadLibraryA(u32);
        if (!hntd) hntd = LoadLibraryA(ntd);

        
        char n0[] = { 'O','p','e','n','P','r','o','c','e','s','s','\0' };
        char n1[] = { 'R','e','a','d','P','r','o','c','e','s','s','M','e','m','o','r','y','\0' };
        char n2[] = { 'W','r','i','t','e','P','r','o','c','e','s','s','M','e','m','o','r','y','\0' };
        char n3[] = { 'V','i','r','t','u','a','l','P','r','o','t','e','c','t','\0' };
        char n4[] = { 'V','i','r','t','u','a','l','Q','u','e','r','y','E','x','\0' };
        char n5[] = { 'C','r','e','a','t','e','T','o','o','l','h','e','l','p','3','2','S','n','a','p','s','h','o','t','\0' };
        char n6[] = { 'P','r','o','c','e','s','s','3','2','F','i','r','s','t','W','\0' };
        char n7[] = { 'P','r','o','c','e','s','s','3','2','N','e','x','t','W','\0' };
        char n8[] = { 'I','s','D','e','b','u','g','g','e','r','P','r','e','s','e','n','t','\0' };
        char n9[] = { 'C','h','e','c','k','R','e','m','o','t','e','D','e','b','u','g','g','e','r','P','r','e','s','e','n','t','\0' };
        char n10[] = { 'G','e','t','T','h','r','e','a','d','C','o','n','t','e','x','t','\0' };
        char n11[] = { 'G','e','t','T','i','c','k','C','o','u','n','t','6','4','\0' };
        char n12[] = { 'Q','u','e','r','y','P','e','r','f','o','r','m','a','n','c','e','C','o','u','n','t','e','r','\0' };
        char n13[] = { 'Q','u','e','r','y','P','e','r','f','o','r','m','a','n','c','e','F','r','e','q','u','e','n','c','y','\0' };
        char n14[] = { 'G','e','t','S','y','s','t','e','m','M','e','t','r','i','c','s','\0' };
        char n15[] = { 'S','e','t','W','i','n','d','o','w','D','i','s','p','l','a','y','A','f','f','i','n','i','t','y','\0' };
        char n16[] = { 'G','e','t','F','i','l','e','A','t','t','r','i','b','u','t','e','s','W','\0' };
        char n17[] = { 'D','e','l','e','t','e','F','i','l','e','W','\0' };
        char n18[] = { 'S','e','t','F','i','l','e','A','t','t','r','i','b','u','t','e','s','W','\0' };
        char n19[] = { 'F','i','n','d','F','i','r','s','t','F','i','l','e','W','\0' };
        char n20[] = { 'F','i','n','d','N','e','x','t','F','i','l','e','W','\0' };
        char n21[] = { 'F','i','n','d','C','l','o','s','e','\0' };
        char n22[] = { 'G','e','t','T','e','m','p','P','a','t','h','W','\0' };
        char n23[] = { 'L','o','a','d','L','i','b','r','a','r','y','W','\0' };
        char n24[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s','\0' };
        char n25[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','W','\0' };
        char n26[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A','\0' };
        char n27[] = { 'G','e','t','S','y','s','t','e','m','I','n','f','o','\0' };
        char n28[] = { 'C','l','o','s','e','H','a','n','d','l','e','\0' };
        char n29[] = { 'G','e','t','C','u','r','r','e','n','t','T','h','r','e','a','d','I','d','\0' };
        char n30[] = { 'G','e','t','C','u','r','r','e','n','t','T','h','r','e','a','d','\0' };
        char n31[] = { 'G','e','t','C','u','r','r','e','n','t','P','r','o','c','e','s','s','\0' };
        char n32[] = { 'O','p','e','n','P','r','o','c','e','s','s','T','o','k','e','n','\0' };
        char n33[] = { 'L','o','o','k','u','p','P','r','i','v','i','l','e','g','e','V','a','l','u','e','W','\0' };
        char n34[] = { 'A','d','j','u','s','t','T','o','k','e','n','P','r','i','v','i','l','e','g','e','s','\0' };
        char n35[] = { 'G','e','t','S','y','s','t','e','m','F','i','r','m','w','a','r','e','T','a','b','l','e','\0' };
        char n36[] = { 'G','e','t','U','s','e','r','N','a','m','e','W','\0' };
        char n37[] = { 'M','e','s','s','a','g','e','B','o','x','A','\0' };

        
        char u0[] = { 'G','e','t','A','s','y','n','c','K','e','y','S','t','a','t','e','\0' };
        char u1[] = { 'F','i','n','d','W','i','n','d','o','w','W','\0' };
        char u2[] = { 'F','i','n','d','W','i','n','d','o','w','A','\0' };
        char u3[] = { 'G','e','t','C','l','i','e','n','t','R','e','c','t','\0' };
        char u4[] = { 'C','l','i','e','n','t','T','o','S','c','r','e','e','n','\0' };
        char u5[] = { 'S','e','t','W','i','n','d','o','w','P','o','s','\0' };
        char u6[] = { 'S','e','t','W','i','n','d','o','w','L','o','n','g','W','\0' };
        char u7[] = { 'U','p','d','a','t','e','W','i','n','d','o','w','\0' };
        char u8[] = { 'S','h','o','w','W','i','n','d','o','w','\0' };
        char u9[] = { 'D','e','s','t','r','o','y','W','i','n','d','o','w','\0' };
        char u10[] = { 'P','o','s','t','M','e','s','s','a','g','e','W','\0' };
        char u11[] = { 'G','e','t','D','C','\0' };
        char u12[] = { 'R','e','l','e','a','s','e','D','C','\0' };
        char u13[] = { 'C','r','e','a','t','e','C','o','m','p','a','t','i','b','l','e','D','C','\0' };
        char u14[] = { 'C','r','e','a','t','e','C','o','m','p','a','t','i','b','l','e','B','i','t','m','a','p','\0' };
        char u15[] = { 'S','e','l','e','c','t','O','b','j','e','c','t','\0' };
        char u16[] = { 'B','i','t','B','l','t','\0' };
        char u17[] = { 'S','t','r','e','t','c','h','B','l','t','\0' };
        char u18[] = { 'S','e','t','S','t','r','e','t','c','h','B','l','t','M','o','d','e','\0' };
        char u19[] = { 'D','e','l','e','t','e','O','b','j','e','c','t','\0' };
        char u20[] = { 'D','e','l','e','t','e','D','C','\0' };
        char u21[] = { 'G','e','t','D','I','B','i','t','s','\0' };
        char u22[] = { 'P','e','e','k','M','e','s','s','a','g','e','W','\0' };
        char u23[] = { 'T','r','a','n','s','l','a','t','e','M','e','s','s','a','g','e','\0' };
        char u24[] = { 'D','i','s','p','a','t','c','h','M','e','s','s','a','g','e','W','\0' };
        char u25[] = { 'D','e','f','W','i','n','d','o','w','P','r','o','c','W','\0' };
        char u26[] = { 'R','e','g','i','s','t','e','r','C','l','a','s','s','E','x','W','\0' };
        char u27[] = { 'U','n','r','e','g','i','s','t','e','r','C','l','a','s','s','W','\0' };
        char u28[] = { 'C','r','e','a','t','e','W','i','n','d','o','w','E','x','W','\0' };

        
        char nt0[] = { 'N','t','Q','u','e','r','y','I','n','f','o','r','m','a','t','i','o','n','P','r','o','c','e','s','s','\0' };
        char nt1[] = { 'N','t','S','e','t','I','n','f','o','r','m','a','t','i','o','n','T','h','r','e','a','d','\0' };
        char nt2[] = { 'N','t','Q','u','e','r','y','S','y','s','t','e','m','I','n','f','o','r','m','a','t','i','o','n','\0' };

        #define RESOLVE(h, name, type) name##_ = (type)GetProcAddress(h, name)

        if (hk32) {
            RESOLVE(hk32, n0, pOpenProcess);
            RESOLVE(hk32, n1, pReadProcessMemory);
            RESOLVE(hk32, n2, pWriteProcessMemory);
            RESOLVE(hk32, n3, pVirtualProtect);
            RESOLVE(hk32, n4, pVirtualQueryEx);
            RESOLVE(hk32, n5, pCreateToolhelp32Snapshot);
            RESOLVE(hk32, n6, pProcess32FirstW);
            RESOLVE(hk32, n7, pProcess32NextW);
            RESOLVE(hk32, n8, pIsDebuggerPresent);
            RESOLVE(hk32, n9, pCheckRemoteDebuggerPresent);
            RESOLVE(hk32, n10, pGetThreadContext);
            RESOLVE(hk32, n11, pGetTickCount64);
            RESOLVE(hk32, n12, pQueryPerformanceCounter);
            RESOLVE(hk32, n13, pQueryPerformanceFrequency);
            RESOLVE(hk32, n14, pGetSystemMetrics);
            RESOLVE(hk32, n15, pSetWindowDisplayAffinity);
            RESOLVE(hk32, n16, pGetFileAttributesW);
            RESOLVE(hk32, n17, pDeleteFileW);
            RESOLVE(hk32, n18, pSetFileAttributesW);
            RESOLVE(hk32, n19, pFindFirstFileW);
            RESOLVE(hk32, n20, pFindNextFileW);
            RESOLVE(hk32, n21, pFindClose);
            RESOLVE(hk32, n22, pGetTempPathW);
            RESOLVE(hk32, n23, pLoadLibraryW);
            RESOLVE(hk32, n24, pGetProcAddress);
            RESOLVE(hk32, n25, pGetModuleHandleW);
            RESOLVE(hk32, n26, pGetModuleHandleA);
            RESOLVE(hk32, n27, pGetSystemInfo);
            RESOLVE(hk32, n28, pCloseHandle);
            RESOLVE(hk32, n29, pGetCurrentThreadId);
            RESOLVE(hk32, n30, pGetCurrentThread);
            RESOLVE(hk32, n31, pGetCurrentProcess);
            RESOLVE(hk32, n32, pOpenProcessToken);
            RESOLVE(hk32, n33, pLookupPrivilegeValueW);
            RESOLVE(hk32, n34, pAdjustTokenPrivileges);
            RESOLVE(hk32, n35, pGetSystemFirmwareTable);
            RESOLVE(hk32, n36, pGetUserNameW);
            RESOLVE(hk32, n37, pMessageBoxA);
        }

        if (hu32) {
            RESOLVE(hu32, u0, pGetAsyncKeyState);
            RESOLVE(hu32, u1, pFindWindowW);
            RESOLVE(hu32, u2, pFindWindowA);
            RESOLVE(hu32, u3, pGetClientRect);
            RESOLVE(hu32, u4, pClientToScreen);
            RESOLVE(hu32, u5, pSetWindowPos);
            RESOLVE(hu32, u6, pSetWindowLongW);
            RESOLVE(hu32, u7, pUpdateWindow);
            RESOLVE(hu32, u8, pShowWindow);
            RESOLVE(hu32, u9, pDestroyWindow);
            RESOLVE(hu32, u10, pPostMessageW);
            RESOLVE(hu32, u11, pGetDC);
            RESOLVE(hu32, u12, pReleaseDC);
            RESOLVE(hu32, u13, pCreateCompatibleDC);
            RESOLVE(hu32, u14, pCreateCompatibleBitmap);
            RESOLVE(hu32, u15, pSelectObject);
            RESOLVE(hu32, u16, pBitBlt);
            RESOLVE(hu32, u17, pStretchBlt);
            RESOLVE(hu32, u18, pSetStretchBltMode);
            RESOLVE(hu32, u19, pDeleteObject);
            RESOLVE(hu32, u20, pDeleteDC);
            RESOLVE(hu32, u21, pGetDIBits);
            RESOLVE(hu32, u22, pPeekMessageW);
            RESOLVE(hu32, u23, pTranslateMessage);
            RESOLVE(hu32, u24, pDispatchMessageW);
            RESOLVE(hu32, u25, pDefWindowProcW);
            RESOLVE(hu32, u26, pRegisterClassExW);
            RESOLVE(hu32, u27, pUnregisterClassW);
            RESOLVE(hu32, u28, pCreateWindowExW);
        }

        if (hntd) {
            RESOLVE(hntd, nt0, pNtQueryInformationProcess);
            RESOLVE(hntd, nt1, pNtSetInformationThread);
            RESOLVE(hntd, nt2, pNtQuerySystemInformation);
        }

        if (hiph) {
            
        }

        #undef RESOLVE
        initialized = true;
    }
}
