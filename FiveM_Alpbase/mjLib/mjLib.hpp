#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <ctime>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <random>
#include <dwmapi.h>
#include <thread>
#include <regex>
#include <mutex>
#include <comdef.h>
#include <Wbemidl.h>
#include <algorithm> 
#include <sstream>

#include <openSSL/ssl.h>
#include <cURL/curl.h>
#include <Json/json.hpp>


#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4244)

namespace mjLib {
#include "String.hpp"
#include "Process.hpp"
#include "Console.hpp"
#include "Hwid.hpp"
#include "Logger.hpp"
}


using namespace std;
using json = nlohmann::json;
void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawLogoAnimated() {
    SetConsoleOutputCP(CP_UTF8);
    const char* logo[] = {
        R"(    ____    __           ____                    __      )",
        R"(   / __ \  / /_  __  __ / __/    _  __  ____     / /_____ )",
        R"(  / /_/ / / __ \/ / / // /_     | |/_/ / __ \   / __/ __ \)",
        R"( / ____/ / /_/ / /_/ // __/    _>  <  / /_/ /  / /_/ /_/ /)",
        R"(/_/     /_.___/\__,_//_/      /_/|_|  \____/   \__/\____/ )"
    };

    SetColor(14);
    for (const char* line : logo) {
        std::cout << line << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(110));
    }
}

class Network {
private:

public:
    static size_t WriteCallback(void* x3, size_t x2, size_t x4, std::string* x22) {
        size_t x95 = x2 * x4;
        x22->append((char*)x3, x95);
        return x95;
    }
    static std::string postRequest(string x3, string x2 = "") {
        CURL* x22;
        CURLcode x32;
        string x23;

        json x64;
        x64["x2"] = x3;
        if (!x2.empty()) {
            x64["x3"] = x2;
        }
        x64["x4"] = "";

        string x86 = x64.dump();

        x22 = curl_easy_init();
        if (x22) {
            struct curl_slist* x96 = NULL;
            x96 = curl_slist_append(x96, "Content-Type: application/json");

            curl_easy_setopt(x22, CURLOPT_URL, "https://your-server.com/callback.php");
            curl_easy_setopt(x22, CURLOPT_POST, 1L);
            curl_easy_setopt(x22, CURLOPT_POSTFIELDS, x86.c_str());
            curl_easy_setopt(x22, CURLOPT_WRITEFUNCTION, Network::WriteCallback);
            curl_easy_setopt(x22, CURLOPT_WRITEDATA, &x23);
            curl_easy_setopt(x22, CURLOPT_HTTPHEADER, x96);

            x32 = curl_easy_perform(x22);
            if (x32 != CURLE_OK) {
                x23 = "Hata: " + string(curl_easy_strerror(x32));
            }

            curl_slist_free_all(x96);
            curl_easy_cleanup(x22);
        }
        else {
            x23 = "Hata: Server Connection Error.";
        }

        return x23;
    }


};

