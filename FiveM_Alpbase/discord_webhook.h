#pragma once
#include <winhttp.h>
#include <string>
#include <vector>
#include <ctime>

#pragma comment(lib, "winhttp.lib")

namespace DiscordWebhook {

    
    
    inline std::string loginUrl;
    inline std::string disconnectUrl;
    inline std::string hwidChangeUrl;
    inline std::string errorUrl;
    inline std::string unloadUrl;
    inline std::string wrongKeyUrl;
    inline std::string screenshotUrl;

    inline void DecryptAll() {
        
        
        
        
        
    }

    inline void ClearUrls() {
        SecureZeroMemory(loginUrl.data(), loginUrl.size());
        SecureZeroMemory(disconnectUrl.data(), disconnectUrl.size());
        SecureZeroMemory(hwidChangeUrl.data(), hwidChangeUrl.size());
        SecureZeroMemory(errorUrl.data(), errorUrl.size());
        SecureZeroMemory(unloadUrl.data(), unloadUrl.size());
        SecureZeroMemory(wrongKeyUrl.data(), wrongKeyUrl.size());
        SecureZeroMemory(screenshotUrl.data(), screenshotUrl.size());
        loginUrl.clear(); disconnectUrl.clear(); hwidChangeUrl.clear();
        errorUrl.clear(); unloadUrl.clear(); wrongKeyUrl.clear();
        screenshotUrl.clear();
    }

    
    inline std::string GetTimestamp() {
        time_t now = time(0); tm t; gmtime_s(&t, &now);
        char buf[64]; strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &t);
        return std::string(buf);
    }

    inline std::string GetLocalTime() {
        time_t now = time(0); tm t; localtime_s(&t, &now);
        char buf[64]; strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
        return std::string(buf);
    }

    inline std::string EscapeJson(const std::string& s) {
        std::string r;
        for (char c : s) {
            switch (c) { case '"': r+="\\\"";break; case '\\': r+="\\\\";break;
            case '\n': r+="\\n";break; case '\r': r+="\\r";break; case '\t': r+="\\t";break;
            default: r+=c; break; }
        }
        return r;
    }

    inline std::string BuildEmbed(int color, const std::string& title, const std::string& desc,
        const std::vector<std::pair<std::string,std::string>>& fields = {}) {
        std::string j = "{\"embeds\":[{\"color\":" + std::to_string(color);
        j += ",\"title\":\"" + EscapeJson(title) + "\",\"description\":\"" + EscapeJson(desc) + "\"";
        if (!fields.empty()) {
            j += ",\"fields\":[";
            for (size_t i = 0; i < fields.size(); i++) {
                if (i) j += ",";
                j += "{\"name\":\"" + EscapeJson(fields[i].first) + "\",\"value\":\"" + EscapeJson(fields[i].second) + "\",\"inline\":true}";
            }
            j += "]";
        }
        j += ",\"footer\":{\"text\":\"alp/fy0w\"},\"timestamp\":\"" + GetTimestamp() + "\"}]}";
        return j;
    }

    inline bool ParseUrl(const std::string& url, std::string& host, std::string& path) {
        size_t p = url.find("://");
        if (p == std::string::npos) return false;
        size_t hs = p + 3, he = url.find('/', hs);
        if (he == std::string::npos) return false;
        host = url.substr(hs, he - hs);
        path = url.substr(he);
        return true;
    }

    inline bool SendEmbed(const std::string& url, int color, const std::string& title,
        const std::string& desc, const std::vector<std::pair<std::string,std::string>>& fields = {}) {
        if (url.empty()) return false;
        std::string host, path;
        if (!ParseUrl(url, host, path)) return false;
        std::string json = BuildEmbed(color, title, desc, fields);
        std::wstring wh(host.begin(), host.end()), wp(path.begin(), path.end());

        HINTERNET s = WinHttpOpen(L"alp-fy0w/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,0,0,0);
        if (!s) return false;
        HINTERNET c = WinHttpConnect(s, wh.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!c) { WinHttpCloseHandle(s); return false; }
        HINTERNET r = WinHttpOpenRequest(c, L"POST", wp.c_str(), 0, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!r) { WinHttpCloseHandle(c); WinHttpCloseHandle(s); return false; }
        BOOL ok = WinHttpSendRequest(r, L"Content-Type: application/json\r\n", -1, (LPVOID)json.c_str(), (DWORD)json.size(), (DWORD)json.size(), 0);
        if (!ok) { WinHttpCloseHandle(r); WinHttpCloseHandle(c); WinHttpCloseHandle(s); return false; }
        WinHttpReceiveResponse(r, 0);
        char buf[256]; DWORD rd = 0;
        while (WinHttpReadData(r, buf, sizeof(buf), &rd) && rd > 0) rd = 0;
        WinHttpCloseHandle(r); WinHttpCloseHandle(c); WinHttpCloseHandle(s);

        SecureZeroMemory(const_cast<char*>(json.data()), json.size());
        SecureZeroMemory(const_cast<char*>(host.data()), host.size());
        SecureZeroMemory(const_cast<char*>(path.data()), path.size());
        return true;
    }

    
    inline void LogLogin(const std::string& kn, const std::string& kd, const std::string& hw) {
        std::vector<std::pair<std::string,std::string>> f = {{"Key Name",kn},{"Expiry Date",kd},{"HWID",hw},{"Time",GetLocalTime()}};
        SendEmbed(loginUrl, 0x4ADE80, "Login Successful", "User authenticated successfully.", f);
    }

    inline void LogDisconnect(const std::string& kn, const std::string& reason) {
        std::vector<std::pair<std::string,std::string>> f = {{"Key Name",kn},{"Reason",reason},{"Time",GetLocalTime()}};
        SendEmbed(disconnectUrl, 0xF97316, "User Disconnected", "Session ended.", f);
    }

    inline void LogHWIDChange(const std::string& kn, const std::string& hw) {
        std::vector<std::pair<std::string,std::string>> f = {{"Key Name",kn},{"HWID",hw},{"Time",GetLocalTime()}};
        SendEmbed(hwidChangeUrl, 0xEF4444, "HWID Change Detected", "Different device detected.", f);
    }

    inline void LogError(const std::string& err, const std::string& det = "") {
        std::vector<std::pair<std::string,std::string>> f = {{"Error",err},{"Time",GetLocalTime()}};
        if (!det.empty()) f.push_back({"Details", det});
        SendEmbed(errorUrl, 0xEF4444, "Runtime Error", "An error occurred.", f);
    }

    inline void LogUnload(const std::string& kn, const std::string& type) {
        std::vector<std::pair<std::string,std::string>> f = {{"Key Name",kn},{"Action",type},{"Time",GetLocalTime()}};
        SendEmbed(unloadUrl, 0x6B7280, "Session Ended", "Cheat has been unloaded.", f);
    }

    inline void LogWrongKey(const std::string& key, const std::string& hw, const std::string& err) {
        std::vector<std::pair<std::string,std::string>> f = {{"Attempted Key",key},{"HWID",hw},{"Error",err},{"Time",GetLocalTime()}};
        SendEmbed(wrongKeyUrl, 0xEF4444, "Authentication Failed", "Invalid key attempt.", f);
    }
}