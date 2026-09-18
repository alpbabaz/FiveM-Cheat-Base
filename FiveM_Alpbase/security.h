#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <intrin.h>
#include <thread>
#include <atomic>
#include <winternl.h>
#include <wintrust.h>
#include <softpub.h>
#include <psapi.h>
#include <cmath>
#include <random>
#include <chrono>
#include <iphlpapi.h>
#include <Iptypes.h>

#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "iphlpapi.lib")

namespace Security {

    inline std::atomic<bool> securityExit{ false };
    inline std::atomic<bool> detected{ false };
    inline std::atomic<int> guardFailCount{ 0 };

    typedef BOOL(WINAPI* pCheckRemoteDbg)(HANDLE, PBOOL);
    typedef BOOL(WINAPI* pGetThreadCtx)(HANDLE, LPCONTEXT);
    typedef NTSTATUS(NTAPI* pNtQueryInformationProcess)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, ULONG, PVOID, ULONG);
    typedef NTSTATUS(NTAPI* pNtQuerySystemInformation)(ULONG, PVOID, ULONG, PULONG);
    typedef NTSTATUS(NTAPI* pNtQueryVirtualMemory)(HANDLE, PVOID, ULONG, PVOID, ULONG, PULONG);
    typedef BOOL(WINAPI* pIsWow64Process)(HANDLE, PBOOL);
    typedef NTSTATUS(NTAPI* pNtClose)(HANDLE);
    typedef NTSTATUS(NTAPI* pNtDuplicateObject)(HANDLE, HANDLE, HANDLE, PHANDLE, ACCESS_MASK, ULONG, ULONG);

    inline pCheckRemoteDbg g_CheckRemoteDbg = nullptr;
    inline pGetThreadCtx g_GetThreadCtx = nullptr;
    inline pNtQueryInformationProcess g_NtQIP = nullptr;
    inline pNtSetInformationThread g_NtSIT = nullptr;
    inline pNtQuerySystemInformation g_NtQSI = nullptr;
    inline pNtQueryVirtualMemory g_NtQVM = nullptr;
    inline pIsWow64Process g_IsWow64 = nullptr;
    inline pNtClose g_NtClose = nullptr;
    inline pNtDuplicateObject g_NtDupObj = nullptr;

    inline void ResolveAPIs() {
        HMODULE k32 = GetModuleHandleA("kernel32.dll");
        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (k32) {
            g_CheckRemoteDbg = (pCheckRemoteDbg)GetProcAddress(k32, "CheckRemoteDebuggerPresent");
            g_GetThreadCtx = (pGetThreadCtx)GetProcAddress(k32, "GetThreadContext");
            g_IsWow64 = (pIsWow64Process)GetProcAddress(k32, "IsWow64Process");
        }
        if (ntdll) {
            g_NtQIP = (pNtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
            g_NtSIT = (pNtSetInformationThread)GetProcAddress(ntdll, "NtSetInformationThread");
            g_NtQSI = (pNtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
            g_NtQVM = (pNtQueryVirtualMemory)GetProcAddress(ntdll, "NtQueryVirtualMemory");
            g_NtClose = (pNtClose)GetProcAddress(ntdll, "NtClose");
            g_NtDupObj = (pNtDuplicateObject)GetProcAddress(ntdll, "NtDuplicateObject");
        }
    }

    inline void HideThread() {
        if (g_NtSIT) g_NtSIT(GetCurrentThread(), 0x11, nullptr, 0);
    }

    
    inline bool CheckDebug() {
        
        if (IsDebuggerPresent()) return true;

        
        if (g_CheckRemoteDbg) {
            BOOL r = FALSE;
            g_CheckRemoteDbg(GetCurrentProcess(), &r);
            if (r) return true;
        }

        
        #ifdef _WIN64
        PBYTE peb = (PBYTE)__readgsqword(0x60);
        #else
        PBYTE peb = (PBYTE)__readfsdword(0x30);
        #endif
        if (peb && peb[2]) return true;

        
        if (peb) {
            DWORD ntGlobalFlag = *(DWORD*)(peb + 0xBC);
            if (ntGlobalFlag & 0x70) return true;
        }

        
        if (g_GetThreadCtx) {
            CONTEXT ctx = {};
            ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
            g_GetThreadCtx(GetCurrentThread(), &ctx);
            if (ctx.Dr0 || ctx.Dr1 || ctx.Dr2 || ctx.Dr3 || ctx.Dr6 || ctx.Dr7) return true;
        }

        
        if (g_NtQIP) {
            DWORD debugPort = 0;
            if (g_NtQIP(GetCurrentProcess(), 7, &debugPort, sizeof(debugPort), nullptr) == 0) {
                if (debugPort != 0) return true;
            }
        }

        
        if (g_NtQIP) {
            HANDLE debugObj = nullptr;
            if (g_NtQIP(GetCurrentProcess(), 0x1E, &debugObj, sizeof(debugObj), nullptr) == 0) {
                if (debugObj) return true;
            }
        }

        
        if (g_NtQIP) {
            DWORD debugFlags = 0;
            if (g_NtQIP(GetCurrentProcess(), 0x1F, &debugFlags, sizeof(debugFlags), nullptr) == 0) {
                if (debugFlags == 0) return true;
            }
        }

        
        {
            auto t1 = __rdtsc();
            volatile int x = 0;
            for (int i = 0; i < 1000; i++) x += i;
            auto t2 = __rdtsc();
            if ((t2 - t1) > 100000) return true;
        }

        
        {
            auto start = std::chrono::high_resolution_clock::now();
            Sleep(10);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            if (duration > 50) return true; 
        }

        
        if (peb) {
            PWSTR debugStr = *(PWSTR*)(peb + 0x70);
            if (debugStr && wcslen(debugStr) > 0) return true;
        }

        return false;
    }

    
    inline bool CheckVM() {
        
        int cpuInfo[4] = { 0 };
        __cpuid(cpuInfo, 1);
        if (cpuInfo[2] & (1 << 31)) return true; 

        
        char vendor[13] = { 0 };
        __cpuid(cpuInfo, 0x40000000);
        memcpy(vendor, &cpuInfo[1], 4);
        memcpy(vendor + 4, &cpuInfo[2], 4);
        memcpy(vendor + 8, &cpuInfo[3], 4);
        
        const char* vmVendors[] = {
            "VMwareVMware", "Microsoft Hv", "KVMKVMKVM", "VBoxVBoxVBox",
            "XenVMMXenVMM", "prl hyperv", " lrpepyh  vr"
        };
        for (auto& v : vmVendors) {
            if (strstr(vendor, v)) return true;
        }

        
        const wchar_t* vmRegKeys[] = {
            L"SOFTWARE\\VMware, Inc.\\VMware Tools",
            L"SOFTWARE\\Oracle\\VirtualBox Guest Additions",
            L"SYSTEM\\CurrentControlSet\\Services\\VBoxGuest",
            L"SYSTEM\\CurrentControlSet\\Services\\vmci",
            L"SYSTEM\\CurrentControlSet\\Services\\vmhgfs",
            L"SYSTEM\\CurrentControlSet\\Services\\VBoxSF",
            L"SYSTEM\\CurrentControlSet\\Services\\VBoxVideo",
        };
        for (auto& key : vmRegKeys) {
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return true;
            }
        }

        
        {
            IP_ADAPTER_INFO adapterInfo[16];
            DWORD bufLen = sizeof(adapterInfo);
            if (GetAdaptersInfo(adapterInfo, &bufLen) == NO_ERROR) {
                PIP_ADAPTER_INFO pAdapter = adapterInfo;
                while (pAdapter) {
                    
                    if (pAdapter->Address[0] == 0x00 && pAdapter->Address[1] == 0x0C && pAdapter->Address[2] == 0x29) return true;
                    if (pAdapter->Address[0] == 0x00 && pAdapter->Address[1] == 0x50 && pAdapter->Address[2] == 0x56) return true;
                    
                    if (pAdapter->Address[0] == 0x08 && pAdapter->Address[1] == 0x00 && pAdapter->Address[2] == 0x27) return true;
                    pAdapter = pAdapter->Next;
                }
            }
        }

        
        {
            ULARGE_INTEGER totalBytes;
            if (GetDiskFreeSpaceExW(L"C:\\", nullptr, &totalBytes, nullptr)) {
                if (totalBytes.QuadPart < 60ULL * 1024 * 1024 * 1024) return true; 
            }
        }

        
        {
            MEMORYSTATUSEX memInfo = { sizeof(memInfo) };
            if (GlobalMemoryStatusEx(&memInfo)) {
                if (memInfo.ullTotalPhys < 4ULL * 1024 * 1024 * 1024) return true; 
            }
        }

        
        const wchar_t* vmProcesses[] = {
            L"vmtoolsd.exe", L"vmwaretray.exe", L"vmwareuser.exe",
            L"VBoxService.exe", L"VBoxTray.exe", L"VBoxClient.exe",
            L"vmacthlp.exe", L"VGAuthService.exe", L"vmware.exe",
        };
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe = { sizeof(pe) };
            if (Process32FirstW(snap, &pe)) {
                do {
                    for (auto& name : vmProcesses) {
                        if (_wcsicmp(pe.szExeFile, name) == 0) {
                            CloseHandle(snap);
                            return true;
                        }
                    }
                } while (Process32NextW(snap, &pe));
            }
            CloseHandle(snap);
        }

        
        {
            system("wmic bios get serialnumber > nul 2>&1");
        }

        return false;
    }

    
    inline bool CheckBlacklist() {
        const wchar_t* list[] = {
            
            L"ida.exe", L"ida64.exe", L"idag.exe", L"idag64.exe",
            L"idaq.exe", L"idaq64.exe", L"idaw.exe", L"idaw64.exe",
            L"idap.exe", L"idap64.exe", L"ida.com", L"ida64.com",
            L"idawin.exe", L"idawin64.exe",
            L"ida_free.exe", L"ida_free64.exe",
            L"idapro.exe", L"idapro64.exe",
            L"ida_pro.exe", L"ida_pro64.exe",
            L"ida9.0.exe", L"ida9.1.exe", L"ida9.2.exe", L"ida9.3.exe", L"ida9.4.exe", L"ida9.5.exe",
            L"ida8.0.exe", L"ida8.1.exe", L"ida8.2.exe", L"ida8.3.exe", L"ida8.4.exe", L"ida8.5.exe",
            L"ida7.0.exe", L"ida7.1.exe", L"ida7.2.exe", L"ida7.3.exe", L"ida7.4.exe", L"ida7.5.exe", L"ida7.6.exe", L"ida7.7.exe", L"ida7.8.exe", L"ida7.9.exe",
            L"ida6.0.exe", L"ida6.1.exe", L"ida6.2.exe", L"ida6.3.exe", L"ida6.4.exe", L"ida6.5.exe", L"ida6.6.exe", L"ida6.7.exe", L"ida6.8.exe", L"ida6.9.exe",
            L"idat.exe", L"idat64.exe", L"idaheadless.exe", L"idaheadless64.exe",
            L"idalib.dll", L"idalib64.dll", L"idapython.dll", L"idapython64.dll",
            
            
            L"ghidra.exe", L"ghidraRun.bat", L"ghidraRun.exe",
            L"ghidra9.0.exe", L"ghidra9.1.exe", L"ghidra9.2.exe", L"ghidra9.3.exe", L"ghidra9.4.exe", L"ghidra9.5.exe",
            L"ghidra10.0.exe", L"ghidra10.1.exe", L"ghidra10.2.exe", L"ghidra10.3.exe", L"ghidra10.4.exe", L"ghidra10.5.exe",
            L"ghidra11.0.exe", L"ghidra11.1.exe", L"ghidra11.2.exe", L"ghidra11.3.exe",
            L"ghidraDebug.exe", L"ghidraDebug.bat",
            L"analyzeHeadless.exe", L"analyzeHeadless.bat",
            L"ghidra-svr.exe", L"ghidra-svr.bat",
            L"Ghidra.exe", L"GhidraDebug.exe",
            L"ghidra.jar", L"ghidra-headless.jar",
            
            
            L"binary.ninja.exe", L"binaryninja.exe", L"binary-ninja.exe",
            L"bn.exe", L"bnd.exe", L"bnu.exe",
            L"binaryninja-personal.exe", L"binaryninja-commercial.exe",
            L"binaryninja-headless.exe",
            L"binary-ninja-1.exe", L"binary-ninja-2.exe", L"binary-ninja-3.exe",
            L"binaryninja-console.exe",
            
            
            L"x64dbg.exe", L"x32dbg.exe", L"x96dbg.exe",
            L"x64dbg_snapshot.exe", L"x32dbg_snapshot.exe",
            L"ollydbg.exe", L"ollydbg2.exe", L"ollydbg201.exe",
            L"debugger.exe", L"debugger64.exe",
            
            
            L"windbg.exe", L"windbgx.exe", L"cdb.exe", L"cdb64.exe",
            L"ntsd.exe", L"ntsd64.exe", L"kd.exe", L"kd64.exe",
            L"dbgsrv.exe", L"dbgsrv64.exe", L"dbghelp.exe", L"dbghelp64.exe",
            L"windbg_preview.exe",
            
            
            L"radare2.exe", L"r2.exe", L"radare2.exe", L"rabin2.exe", L"rafind2.exe",
            L"rahash2.exe", L"rasm2.exe", L"rax2.exe", L"rafind2.exe",
            L"cutter.exe", L"cutter2.exe",
            L"rizin.exe", L"rizin2.exe", L"rz-bin.exe", L"rz-find.exe",
            L"rz-hash.exe", L"rz-asm.exe", L"rz-ax.exe",
            L"rz-diff.exe", L"rz-sign.exe", L"rz-find.exe",
            
            
            L"immunitydebugger.exe", L"immunity.exe",
            L"immunitydebugger64.exe",
            
            
            L"processhacker.exe", L"processhacker2.exe", L"processhacker3.exe",
            L"ph.exe", L"ph64.exe", L"ph32.exe",
            L"systeminformer.exe", L"systeminformer64.exe", L"systeminformer32.exe",
            L"ksd.exe", L"ksd64.exe",
            
            
            L"cheatengine.exe", L"cheatengine-x86_64.exe", L"ce.exe",
            L"cheatengine-i386.exe", L"cheatengine-x86.exe",
            L"cheatengine7.0.exe", L"cheatengine7.1.exe", L"cheatengine7.2.exe",
            L"cheatengine7.3.exe", L"cheatengine7.4.exe", L"cheatengine7.5.exe",
            L"celua.exe", L"celua64.exe",
            
            
            L"wireshark.exe", L"tshark.exe", L"tcpview.exe",
            L"fiddler.exe", L"fiddler2.exe", L"fiddler4.exe", L"fiddler5.exe",
            L"charles.exe", L"charles4.exe",
            L"HTTPDebuggerUI.exe", L"HTTPDebuggerSvc.exe", L"HTTPDebuggerPro.exe",
            L"HTTPDebugger.exe", L"HTTPDebugger64.exe",
            L"dumpcap.exe", L"networkminer.exe",
            L"apimonitor.exe", L"rohitab.com",
            L"spyxx.exe", L"spyxx_amd64.exe",
            L"api-monitor-v2.exe", L"apimonitor.exe",
            L"api-monitor.exe", L"api-monitor64.exe",
            L"detours.exe", L"easyhook.exe",
            L"minhook.exe", L"subhook.exe",
            
            
            L"recaf.exe", L"recaf2.exe", L"recaf3.exe",
            L"jd-gui.exe", L"jd-gui.jar",
            L"cfr.exe", L"cfr.jar",
            L"procyon.exe", L"procyon.jar",
            L"fernflower.exe", L"fernflower.jar",
            L"dnspy.exe", L"dnspy64.exe", L"dnSpy.exe", L"dnSpy64.exe",
            L"ilspy.exe", L"ilspy64.exe", L"ILSpy.exe", L"ILSpy64.exe",
            L"dotpeek.exe", L"dotpeek64.exe", L"dotPeek.exe", L"dotPeek64.exe",
            L"reflexil.exe", L"de4dot.exe", L"de4dot64.exe",
            L"bytecodeviewer.exe", L"bcv.exe",
            L"apktool.exe", L"apktool64.exe",
            L"jadx.exe", L"jadx-gui.exe",
            L"hopper.exe", L"hopperv4.exe",
            L"hopperdisassembler.exe",
            L"retdec.exe", L"retdec-decompiler.exe",
            L"uncompyle6.exe", L"pycdc.exe", L"pycdas.exe",
            L"snowman.exe", L"optimice.exe",
            L"rizin.exe", L"cutter.exe",
            L"binaryai.exe", L"bindiff.exe",
            L"diaphora.exe", L"diaphora64.exe",
            
            
            L"sandboxie.exe", L"sandboxierpcss.exe",
            L"sandboxie.exe", L"sandboxie64.exe",
            L"sbiesvc.exe", L"sandboxiecrypto.exe",
            L"Sandboxie.exe", L"Sandboxie64.exe",
            L"cuckoo.exe", L"cuckoomon.exe", L"cuckoo-agent.exe",
            L"cuckoo64.exe", L"cuckoo-monitor.exe",
            L"any.run.exe", L"anyrun.exe",
            L"hybrid-analysis.exe",
            L"cape.exe", L"cape-agent.exe",
            L"joebox.exe", L"joeboxserver.exe", L"joeboxcontrol.exe",
            L"vmray.exe", L"vmray-agent.exe",
            L"triage.exe", L"triage-agent.exe",
            L"detonation.exe",
            
            
            L"frida.exe", L"frida-server.exe", L"frida-agent.exe",
            L"frida-inject.exe", L"frida-trace.exe", L"frida-discover.exe",
            L"frida-compile.exe", L"frida-create.exe", L"frida-kill.exe",
            L"frida-ls-devices.exe", L"frida-ps.exe",
            L"detours.exe", L"easyhook.exe",
            L"minhook.exe", L"subhook.exe",
            L"detours64.exe", L"easyhook64.exe",
            
            
            L"artmoney.exe", L"artmoney8.exe",
            L"gameguardian.exe", L"gg.exe",
            L"wemod.exe", L"wemod64.exe",
            L"flingtrainer.exe", L"fling.exe",
            L"trainers.exe", L"megadev.exe",
            L"scanmem.exe", L"gameconqueror.exe",
            L"mem_edit.exe", L"mem_edit64.exe",
            L"hexedit.exe", L"hexedit64.exe",
            L"hxd.exe", L"hxd64.exe",
            L"010editor.exe", L"010editor64.exe",
            L"winhex.exe", L"winhex64.exe",
            L"hexworkshop.exe",
            
            
            L"objdump.exe", L"objdump64.exe",
            L"dumpbin.exe", L"dumpbin64.exe",
            L"readelf.exe", L"readelf64.exe",
            L"nm.exe", L"nm64.exe",
            L"string.exe", L"string64.exe",
            L"strings.exe", L"strings64.exe",
            L"capstone.exe", L"capstone64.exe",
            L"keystone.exe", L"keystone64.exe",
            L"unicorn.exe", L"unicorn64.exe",
            
            
            L"decompyle.exe", L"decompyle64.exe",
            L"unpyc.exe", L"uncompyle6.exe",
            L"pycdc.exe", L"pycdas.exe",
            L"de4dot.exe", L"de4dot64.exe",
            
            
            L"symsrv.dll", L"dbghelp.dll",
            L"dbgcore.dll", L"dbghelp64.dll",
            
            
            L"volatility.exe", L"volatility64.exe",
            L"vol.py", L"volatility3.exe",
            L"rekall.exe", L"rekall64.exe",
            L"yara.exe", L"yara64.exe",
            L"yarac.exe", L"yarac64.exe",
            L"yara-python.exe",
            
            
            L"pestudio.exe", L"pestudio64.exe",
            L"pe-bear.exe", L"pe-bear64.exe",
            L"pe-sieve.exe", L"pe-sieve64.exe",
            L"hollows_hunter.exe", L"hollows_hunter64.exe",
            L"procdot.exe", L"procdot64.exe",
            L"procmon.exe", L"procmon64.exe",
            L"procexp.exe", L"procexp64.exe",
            L"autoruns.exe", L"autoruns64.exe",
            L"tcpview.exe", L"tcpview64.exe",
            L"filemon.exe", L"filemon64.exe",
            L"regmon.exe", L"regmon64.exe",
            
            
            L"SandboxiePlus.exe", L"SandboxieMinus.exe",
            L"SandboxieControl.exe", L"SandboxieWUAU.exe",
            L"SandMan.exe", L"SandMan64.exe",
        };
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap == INVALID_HANDLE_VALUE) return false;
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(snap, &pe)) {
            do {
                for (auto& name : list) {
                    if (_wcsicmp(pe.szExeFile, name) == 0) {
                        CloseHandle(snap);
                        return true;
                    }
                }
            } while (Process32NextW(snap, &pe));
        }
        CloseHandle(snap);
        return false;
    }

    
    inline void AntiDump() {
        HMODULE hMod = GetModuleHandle(NULL);
        if (!hMod) return;

        
        DWORD old;
        if (VirtualProtect(hMod, 0x1000, PAGE_READWRITE, &old)) {
            SecureZeroMemory(hMod, 0x1000);
            VirtualProtect(hMod, 0x1000, old, &old);
        }

        
        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hMod;
        if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
            PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hMod + dosHeader->e_lfanew);
            if (ntHeaders->Signature == IMAGE_NT_SIGNATURE) {
                PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
                for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
                    if (VirtualProtect(section, sizeof(IMAGE_SECTION_HEADER), PAGE_READWRITE, &old)) {
                        SecureZeroMemory(section->Name, IMAGE_SIZEOF_SHORT_NAME);
                        VirtualProtect(section, sizeof(IMAGE_SECTION_HEADER), old, &old);
                    }
                    section++;
                }
            }
        }

        
        MEMORY_BASIC_INFORMATION mbi;
        LPVOID addr = hMod;
        while (VirtualQuery(addr, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
                if (mbi.Protect == PAGE_EXECUTE_READWRITE) {
                    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READ, &old);
                }
            }
            addr = (LPVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
        }
    }

    
    inline bool CheckHooks() {
        
        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll) return false;

        const char* criticalFuncs[] = {
            "NtQueryInformationProcess", "NtSetInformationThread",
            "NtQuerySystemInformation", "IsDebuggerPresent",
            "CheckRemoteDebuggerPresent", "NtQueryVirtualMemory",
        };

        for (auto& funcName : criticalFuncs) {
            FARPROC func = GetProcAddress(ntdll, funcName);
            if (func) {
                BYTE* bytes = (BYTE*)func;
                
                if (bytes[0] == 0xE9 || bytes[0] == 0xE8) return true;
                
                if (bytes[0] == 0x48 && bytes[1] == 0xB8 && bytes[10] == 0xFF && bytes[11] == 0xE0) return true;
                
                if (bytes[0] == 0xCC) return true;
            }
        }
        return false;
    }

    
    inline void ProtectMemory() {
        HMODULE hMod = GetModuleHandle(NULL);
        if (!hMod) return;

        
        MEMORY_BASIC_INFORMATION mbi;
        LPVOID addr = hMod;
        while (VirtualQuery(addr, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && mbi.Type == MEM_IMAGE) {
                DWORD old;
                
                if (mbi.Protect == PAGE_EXECUTE_READ) {
                    VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE, &old);
                }
            }
            addr = (LPVOID)((BYTE*)mbi.BaseAddress + mbi.RegionSize);
        }
    }

    
    inline DWORD CalculateChecksum() {
        HMODULE hMod = GetModuleHandle(NULL);
        if (!hMod) return 0;

        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hMod;
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return 0;

        PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hMod + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) return 0;

        DWORD checksum = 0;
        PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
        for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
            if (section->SizeOfRawData > 0) {
                BYTE* data = (BYTE*)hMod + section->VirtualAddress;
                for (DWORD j = 0; j < section->SizeOfRawData; j += 4) {
                    if (j + 4 <= section->SizeOfRawData) {
                        checksum ^= *(DWORD*)(data + j);
                        checksum = _rotl(checksum, 7);
                    }
                }
            }
            section++;
        }
        return checksum;
    }

    inline DWORD g_originalChecksum = 0;

    inline bool CheckIntegrity() {
        if (g_originalChecksum == 0) {
            g_originalChecksum = CalculateChecksum();
            return true;
        }
        return CalculateChecksum() == g_originalChecksum;
    }

    
    inline void EraseHeaders() {
        HMODULE hMod = GetModuleHandle(NULL);
        if (!hMod) return;
        DWORD old;
        if (VirtualProtect(hMod, 0x1000, PAGE_READWRITE, &old)) {
            SecureZeroMemory(hMod, 0x1000);
            VirtualProtect(hMod, 0x1000, old, &old);
        }
    }

    inline void AddJunkCode() {
        volatile int x = 0;
        volatile int y = 0;
        volatile int z = 0;
        for (volatile int i = 0; i < 100; i++) {
            x += i * 7;
            y += x ^ 0xDEAD;
            z += y + i;
        }
        for (volatile int i = 0; i < 50; i++) {
            volatile int arr[10];
            for (int j = 0; j < 10; j++) arr[j] = rand() % 1000;
            for (int j = 0; j < 9; j++) {
                if (arr[j] > arr[j + 1]) {
                    int tmp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = tmp;
                }
            }
        }
        (void)x; (void)y; (void)z;
    }

    
    inline void SelfDelete() {
        wchar_t dllPath[MAX_PATH] = {};
        GetModuleFileNameW(GetModuleHandle(NULL), dllPath, MAX_PATH);
        wchar_t batPath[MAX_PATH] = {};
        GetTempPathW(MAX_PATH, batPath);
        wcscat_s(batPath, L"nx_cleanup.bat");
        FILE* f = nullptr;
        _wfopen_s(&f, batPath, L"w");
        if (f) {
            fwprintf(f, L"@echo off\n");
            fwprintf(f, L"timeout /t 2 /nobreak >nul\n");
            fwprintf(f, L"del /f /q \"%s\" 2>nul\n", dllPath);
            fwprintf(f, L"del /f /q \"%s\" 2>nul\n", batPath);
            fclose(f);
            ShellExecuteW(NULL, L"open", batPath, NULL, NULL, SW_HIDE);
        }
    }

    inline void ScanAndDeleteCopies() {
        wchar_t dllPath[MAX_PATH] = {};
        GetModuleFileNameW(GetModuleHandle(NULL), dllPath, MAX_PATH);
        wchar_t userName[MAX_PATH] = {};
        DWORD userNameLen = MAX_PATH;
        GetUserNameW(userName, &userNameLen);
        wchar_t userDir[MAX_PATH] = {};
        swprintf_s(userDir, L"C:\\Users\\%s", userName);

        
        wchar_t recentPath[MAX_PATH];
        swprintf_s(recentPath, L"%s\\AppData\\Roaming\\Microsoft\\Windows\\Recent", userDir);
        const wchar_t* patterns[] = { L"fy0w", L"auth", L"obs-text", L"AuthServer", L"CFXBASE", L"Alp Base" };
        WIN32_FIND_DATAW rfd = {};
        HANDLE rFind = FindFirstFileW((std::wstring(recentPath) + L"\\*.lnk").c_str(), &rfd);
        if (rFind != INVALID_HANDLE_VALUE) {
            do {
                for (auto& pat : patterns) {
                    if (wcsstr(rfd.cFileName, pat)) {
                        wchar_t fullPath[MAX_PATH];
                        swprintf_s(fullPath, L"%s\\%s", recentPath, rfd.cFileName);
                        DeleteFileW(fullPath);
                        break;
                    }
                }
            } while (FindNextFileW(rFind, &rfd));
            FindClose(rFind);
        }

        
        wchar_t psHist[MAX_PATH];
        swprintf_s(psHist, L"%s\\AppData\\Roaming\\Microsoft\\Windows\\PowerShell\\PSReadLine\\ConsoleHost_history.txt", userDir);
        HANDLE hFile = CreateFileW(psHist, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD fileSize = GetFileSize(hFile, NULL);
            if (fileSize > 0 && fileSize < 1024 * 1024) {
                std::vector<char> buf(fileSize + 1, 0);
                DWORD bytesRead = 0;
                ReadFile(hFile, buf.data(), fileSize, &bytesRead, NULL);
                std::string content(buf.data(), bytesRead);
                CloseHandle(hFile);
                std::string cleaned;
                size_t pos = 0;
                while (pos < content.size()) {
                    size_t lineEnd = content.find('\n', pos);
                    if (lineEnd == std::string::npos) lineEnd = content.size();
                    std::string line = content.substr(pos, lineEnd - pos);
                    if (line.find("fy0w") == std::string::npos &&
                        line.find("obs-text") == std::string::npos &&
                        line.find("AuthServer") == std::string::npos &&
                        line.find("CFXBASE") == std::string::npos &&
                        line.find("Alp Base") == std::string::npos) {
                        cleaned += line + "\n";
                    }
                    pos = lineEnd + 1;
                }
                hFile = CreateFileW(psHist, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
                if (hFile != INVALID_HANDLE_VALUE) {
                    DWORD written = 0;
                    WriteFile(hFile, cleaned.c_str(), (DWORD)cleaned.size(), &written, NULL);
                    CloseHandle(hFile);
                }
            }
            else { CloseHandle(hFile); }
        }

        
        wchar_t cachePath[MAX_PATH];
        swprintf_s(cachePath, L"%s\\AppData\\Local\\Microsoft\\Windows\\Explorer\\iconcache_*.db", userDir);
        WIN32_FIND_DATAW ifd = {};
        HANDLE iFind = FindFirstFileW(cachePath, &ifd);
        if (iFind != INVALID_HANDLE_VALUE) {
            do {
                wchar_t fullPath[MAX_PATH];
                swprintf_s(fullPath, L"%s\\AppData\\Local\\Microsoft\\Windows\\Explorer\\%s", userDir, ifd.cFileName);
                DeleteFileW(fullPath);
            } while (FindNextFileW(iFind, &ifd));
            FindClose(iFind);
        }

        
        if (OpenClipboard(NULL)) { EmptyClipboard(); CloseClipboard(); }
    }

    inline void EmergencyExit() {
        ScanAndDeleteCopies();
        SelfDelete();
        ExitProcess(0);
    }

    
    inline void GuardThread1() {
        
        while (!securityExit) {
            Sleep(1000);
            if (CheckDebug()) { guardFailCount++; if (guardFailCount >= 3) { detected = true; break; } }
            if (CheckVM()) { guardFailCount++; if (guardFailCount >= 3) { detected = true; break; } }
        }
    }

    inline void GuardThread2() {
        
        while (!securityExit) {
            Sleep(1500);
            if (CheckBlacklist()) { guardFailCount++; if (guardFailCount >= 3) { detected = true; break; } }
            if (CheckHooks()) { guardFailCount++; if (guardFailCount >= 3) { detected = true; break; } }
        }
    }

    inline void GuardThread3() {
        
        while (!securityExit) {
            Sleep(2000);
            if (!CheckIntegrity()) { detected = true; break; }
        }
    }

    inline void GuardThread4() {
        
        while (!securityExit) {
            Sleep(3000);
            AntiDump();
            AddJunkCode();
        }
    }

    
    
    
    __declspec(noinline) static void __stdcall FakeExport1() {
        __ud2(); 
    }
    
    __declspec(noinline) static void __stdcall FakeExport2() {
        volatile int x = 0;
        if (x) { 
            __debugbreak();
            *((volatile int*)0) = 0; 
        }
    }
    
    __declspec(noinline) static void __stdcall FakeExport3() {
        
        volatile int a = 7;
        volatile int b = 13;
        volatile int c = a * b;
        if (c * c + 1 == 0) { 
            __fastfail(0);
        }
    }

    
    inline void InsertAntiDecompile() {
        
        HMODULE hMod = GetModuleHandle(NULL);
        if (!hMod) return;
        
        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hMod;
        if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return;
        
        PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hMod + dosHeader->e_lfanew);
        if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) return;
        
        PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
        for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++) {
            if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
                
                DWORD old;
                BYTE* codeStart = (BYTE*)hMod + section->VirtualAddress;
                if (VirtualProtect(codeStart, section->SizeOfRawData, PAGE_EXECUTE_READWRITE, &old)) {
                    
                    for (DWORD j = 0; j < 16; j++) {
                        codeStart[j] = 0xCC;
                    }
                    VirtualProtect(codeStart, section->SizeOfRawData, old, &old);
                }
            }
            section++;
        }
    }

    
    __declspec(noinline) static int AntiDisasm1() {
        volatile int x = __rdtsc() & 1;
        if (x) {
            return 1;
        } else {
            return 0;
        }
        
        __nop(); __nop(); __nop(); __nop();
        __nop(); __nop(); __nop(); __nop();
        return -1;
    }

    __declspec(noinline) static int AntiDisasm2() {
        
        volatile BYTE code[] = { 0xEB, 0x01, 0x90, 0xE8 }; 
        volatile int result = 0;
        for (int i = 0; i < 4; i++) {
            result += code[i];
        }
        return result;
    }

    
    typedef BOOL(WINAPI* pFakeFunc1)(LPVOID);
    typedef NTSTATUS(NTAPI* pFakeFunc2)(HANDLE, ULONG, PVOID, ULONG, PULONG);
    
    inline void CreateFakeImports() {
        
        volatile pFakeFunc1 fake1 = nullptr;
        volatile pFakeFunc2 fake2 = nullptr;
        
        
        HMODULE fakeMod = GetModuleHandleA("kernel32.dll");
        if (fakeMod) {
            fake1 = (pFakeFunc1)GetProcAddress(fakeMod, "HeapQueryInformation");
            fake2 = (pFakeFunc2)GetProcAddress(fakeMod, "HeapQueryInformation");
        }
    }

    
    __declspec(noinline) static void OpaquePredicate1() {
        volatile int x = 42;
        volatile int y = x * x;
        volatile int z = y % 7;
        
        
        if (z * z + z + 1 == 0) {
            volatile int* p = nullptr;
            *p = 0xDEAD;
        }
        
        
        if ((x * 2) + 1 > 0) {
            __nop();
        }
    }

    __declspec(noinline) static void OpaquePredicate2() {
        
        volatile int64_t t = __rdtsc();
        volatile int x = (int)(t & 0xFF);
        
        if ((x * x + 2 * x + 1) == (x + 1) * (x + 1)) {
            __nop();
        }
    }

    
    inline void AntiEmulation() {
        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll) return;
        
        
        typedef NTSTATUS(NTAPI* pNtRaiseHardError)(NTSTATUS, ULONG, ULONG, PULONG_PTR, ULONG, PULONG);
        typedef NTSTATUS(NTAPI* pNtAllocateVirtualMemory)(HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG);
        
        auto NtRaiseHardError = (pNtRaiseHardError)GetProcAddress(ntdll, "NtRaiseHardError");
        auto NtAllocateVirtualMemory = (pNtAllocateVirtualMemory)GetProcAddress(ntdll, "NtAllocateVirtualMemory");
        
        
        if (NtRaiseHardError && NtAllocateVirtualMemory) {
            __nop();
        }
    }

    
    __declspec(noinline) static void FlattenControlFlow(volatile int input) {
        volatile int state = input % 5;
        volatile bool running = true;
        
        while (running) {
            switch (state) {
                case 0:
                    state = 1;
                    break;
                case 1:
                    OpaquePredicate1();
                    state = 2;
                    break;
                case 2:
                    AntiDisasm1();
                    state = 3;
                    break;
                case 3:
                    OpaquePredicate2();
                    state = 4;
                    break;
                case 4:
                    running = false;
                    break;
                default:
                    state = 0;
                    break;
            }
        }
    }

    
    inline void InitAntiAnalysis() {
        InsertAntiDecompile();
        CreateFakeImports();
        AntiEmulation();
        
        
        OpaquePredicate1();
        OpaquePredicate2();
        
        
        FlattenControlFlow(0);
    }

    
    inline void MonitorThread() {
        ResolveAPIs();
        HideThread();
        
        
        g_originalChecksum = CalculateChecksum();
        
        
        EraseHeaders();
        AntiDump();
        ProtectMemory();
        InitAntiAnalysis();

        
        std::thread(GuardThread1).detach();
        std::thread(GuardThread2).detach();
        std::thread(GuardThread3).detach();
        std::thread(GuardThread4).detach();

        
        while (!securityExit) {
            Sleep(500);
            if (detected) {
                EmergencyExit();
                break;
            }
            
            AddJunkCode();
        }
    }

    inline void StartMonitor() {
        std::thread(MonitorThread).detach();
    }

    inline void Shutdown() {
        securityExit = true;
    }
}
