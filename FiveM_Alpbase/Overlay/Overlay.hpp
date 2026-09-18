#pragma once
#include "../AmGui/smoke_effect.h"
#include <chrono>

struct OverlayStruct {
    HWND hWnd;
    WNDCLASSEX wndClassEx;
    LPCSTR lpClassName = "Spotify";
    LPCSTR lpWindowName = "Spotify";
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pDeviceContext;
    IDXGISwapChain* pSwapChain;
    ID3D11RenderTargetView* pRenderTargetView;
    POINT WindowSize;
} Overlay;

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void OverlayMain() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    initializeMenu();
    ImGui_ImplWin32_Init(Overlay.hWnd);
    ImGui_ImplDX11_Init(Overlay.pDevice, Overlay.pDeviceContext);

    static bool streamproofApplied = false;
    static bool splashDone = false;
    static float splashProgress = 0.f;
    static float splashStartTime = 0.f;
    static float splashEndTime = 0.f;
    static bool streamproofReady = false;

    const float TARGET_FPS = 60.f;
    const float FRAME_TIME = 1.f / TARGET_FPS;
    static auto lastFrameTime = std::chrono::high_resolution_clock::now();

    while (!exitLoop) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        if (deltaTime < FRAME_TIME) {
            Sleep(static_cast<DWORD>((FRAME_TIME - deltaTime) * 1000.f));
            continue;
        }
        lastFrameTime = currentTime;

        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (splashDone && !destructActive && GetAsyncKeyState(Cheats::menuKey) & 1) {
            isMenuVisible = !isMenuVisible;
            ImGui::GetIO().MouseDrawCursor = isMenuVisible;
            SetWindowLong(Overlay.hWnd, GWL_EXSTYLE, isMenuVisible ? WS_EX_TRANSPARENT : (WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW));
            UpdateWindow(Overlay.hWnd);
        }

        if (GetAsyncKeyState(Cheats::Misc::NoClipKey) & 1) {
            Cheats::Misc::NoClip = !Cheats::Misc::NoClip;
        }

        if (splashDone && !streamproofReady) {
            if ((float)ImGui::GetTime() - splashEndTime >= 5.0f) {
                streamproofReady = true;
            }
        }

        if (Cheats::Settings::streamproof && streamproofReady && !streamproofApplied) {
            SetWindowDisplayAffinity(Overlay.hWnd, WDA_EXCLUDEFROMCAPTURE);
            streamproofApplied = true;
        }
        else if (!Cheats::Settings::streamproof && streamproofApplied) {
            SetWindowDisplayAffinity(Overlay.hWnd, WDA_NONE);
            streamproofApplied = false;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (!splashDone) {
            if (splashStartTime == 0.f)
                splashStartTime = (float)ImGui::GetTime();

            float elapsed = (float)ImGui::GetTime() - splashStartTime;
            splashProgress = ImClamp(elapsed / 3.0f, 0.f, 1.f);

            if (splashProgress >= 1.f) {
                splashDone = true;
                isMenuVisible = true;
                ImGui::GetIO().MouseDrawCursor = true;
                splashEndTime = (float)ImGui::GetTime();
                std::thread([]() {
                    Beep(1000, 120);
                    Sleep(80);
                    Beep(1000, 120);
                }).detach();
            }

            ImVec2 screen(io.DisplaySize.x, io.DisplaySize.y);
            ImDrawList* fg = ImGui::GetForegroundDrawList();

            fg->AddRectFilled(ImVec2(0, 0), screen, ImGui::GetColorU32(ImVec4(0.f, 0.f, 0.f, 1.f)));

            if (texture::logo_image) {
                float logo_size = 180.f;
                ImVec2 logo_center(screen.x * 0.5f, screen.y * 0.42f);
                ImVec2 logoMin(logo_center.x - logo_size * 0.5f, logo_center.y - logo_size * 0.5f);
                ImVec2 logoMax(logo_center.x + logo_size * 0.5f, logo_center.y + logo_size * 0.5f);

                float glowAlpha = 0.3f + sinf(elapsed * 2.f) * 0.1f;
                fg->AddImageRounded((ImTextureID)texture::logo_image,
                    ImVec2(logoMin.x - 8, logoMin.y - 8), ImVec2(logoMax.x + 8, logoMax.y + 8),
                    ImVec2(0, 0), ImVec2(1, 1),
                    ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, glowAlpha)), logo_size * 0.5f);

                fg->AddImageRounded((ImTextureID)texture::logo_image,
                    logoMin, logoMax,
                    ImVec2(0, 0), ImVec2(1, 1),
                    ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), logo_size * 0.5f);
            }

            ImGui::PushFont(font::inter_semibold);
            const char* injText = "alp/fy0w Injected";
            ImVec2 injSize = ImGui::CalcTextSize(injText);
            float injY = screen.y * 0.42f + 100.f;
            fg->AddText(ImVec2(screen.x * 0.5f - injSize.x * 0.5f, injY),
                ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.9f)), injText);
            ImGui::PopFont();

            ImGui::PushFont(font::regular_m);
            char welcomeBuf[128];
            snprintf(welcomeBuf, sizeof(welcomeBuf), "Welcome, %s", Cheats::Auth::keyName.c_str());
            ImVec2 welcomeSize = ImGui::CalcTextSize(welcomeBuf);
            float welcomeY = injY + 28.f;
            fg->AddText(ImVec2(screen.x * 0.5f - welcomeSize.x * 0.5f, welcomeY),
                ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.9f)), welcomeBuf);
            ImGui::PopFont();

            float barW = 260.f;
            float barH = 3.f;
            float barY = welcomeY + 36.f;
            float barX = screen.x * 0.5f - barW * 0.5f;

            fg->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + barW, barY + barH),
                ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.f)), barH * 0.5f);

            float fillW = barW * splashProgress;
            if (fillW > 0.f) {
                fg->AddRectFilled(ImVec2(barX, barY), ImVec2(barX + fillW, barY + barH),
                    ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 0.9f)), barH * 0.5f);
            }

            ImGui::PushFont(font::small_font);
            char pctBuf[16];
            snprintf(pctBuf, sizeof(pctBuf), "%d%%", (int)(splashProgress * 100.f));
            ImVec2 pctSize = ImGui::CalcTextSize(pctBuf);
            fg->AddText(ImVec2(screen.x * 0.5f - pctSize.x * 0.5f, barY + barH + 8.f),
                ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.8f)), pctBuf);
            ImGui::PopFont();
        }

        static bool smokeInitDone = false;
        if (!smokeInitDone) {
            smoke::Init(Overlay.pDevice);
            smokeInitDone = true;
        }
        smoke::Render(Overlay.pDevice, Overlay.pDeviceContext, (float)GetTickCount64() / 1000.0f);

        DrawAlways();

        if (destructActive) {
            if (!destructCleanupDone) {
                Destruct::PerformCleanup();
                destructCleanupDone = true;
                destructStartTime = (float)ImGui::GetTime();
            }
            float elapsed = (float)ImGui::GetTime() - destructStartTime;
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            Destruct::DrawScreen(elapsed, ImVec2(io.DisplaySize.x, io.DisplaySize.y), fg);
            if (elapsed >= Destruct::duration) {
                Destruct::KillOBS();
                exitLoop = true;
            }
        }
        else if (splashDone) {
            DrawEsp();
            vehicleEsp();

            if (isMenuVisible) {
                drawMenu();
            }
        }

        if (Cheats::Settings::watermark && !destructActive) {
            ImDrawList* fg = ImGui::GetForegroundDrawList();
            float fps = 1.0f / ImMax(io.DeltaTime, 0.00001f);

            char wmBuf[256];
            snprintf(wmBuf, sizeof(wmBuf), "alp/fy0w  |  %s  |  %s  |  %.0f FPS", Cheats::Auth::keyName.c_str(), Cheats::Auth::keyDate.c_str(), fps);

            ImGui::PushFont(font::small_font);
            ImVec2 textSize = ImGui::CalcTextSize(wmBuf);
            ImGui::PopFont();

            ImVec2 screenSize(io.DisplaySize.x, io.DisplaySize.y);
            float padX = 14.f, padY = 8.f;
            float wmW = textSize.x + padX * 2.f;
            float wmH = textSize.y + padY * 2.f;
            float wmX = screenSize.x - wmW - 16.f;
            float wmY = 16.f;

            fg->AddRectFilled(ImVec2(wmX, wmY), ImVec2(wmX + wmW, wmY + wmH), ImGui::GetColorU32(ImVec4(0.06f, 0.06f, 0.06f, 0.85f)), wmH / 2.f);
            fg->AddRect(ImVec2(wmX, wmY), ImVec2(wmX + wmW, wmY + wmH), ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 0.5f)), wmH / 2.f);

            ImGui::PushFont(font::small_font);
            fg->AddText(ImVec2(wmX + padX, wmY + padY), ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.95f)), wmBuf);
            ImGui::PopFont();
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
        Overlay.pDeviceContext->OMSetRenderTargets(1, &Overlay.pRenderTargetView, NULL);
        Overlay.pDeviceContext->ClearRenderTargetView(Overlay.pRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        Overlay.pSwapChain->Present(Cheats::Settings::vsync ? 1 : 0, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
}
#define WDA_NONE 0x00000000
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
void UpdateOverlay() {
    while (!exitLoop) {
        Sleep(3000);
        if (!Process::Check(Game.pID)) {
            break;
        }

        Game.hWnd = FindWindow(Game.lpClassName, NULL);
        if (!Game.hWnd) {
            break;
        }
        DWORD currentAffinity = NULL;
        GetWindowDisplayAffinity(Overlay.hWnd, &currentAffinity);
        RECT currentRect{};
        POINT currentPoint{};
        GetClientRect(Game.hWnd, &currentRect);
        ClientToScreen(Game.hWnd, &currentPoint);
        if (currentRect.left != Game.lpRect.left || currentRect.right != Game.lpRect.right || currentRect.top != Game.lpRect.top || currentRect.bottom != Game.lpRect.bottom || currentPoint.x != Game.lpPoint.x || currentPoint.y != Game.lpPoint.y) {
            Game.lpRect = currentRect;
            Game.lpPoint = currentPoint;
            SetWindowPos(Overlay.hWnd, nullptr, currentPoint.x, currentPoint.y, currentRect.right - currentRect.left, currentRect.bottom - currentRect.top, SWP_NOREDRAW);
        }
    }
}

bool CreateOverlay() {
    WNDCLASSEX wndClassEx;
    ZeroMemory(&wndClassEx, sizeof(wndClassEx));
    wndClassEx.cbSize = sizeof(WNDCLASSEX);
    wndClassEx.lpfnWndProc = WndProc;
    wndClassEx.hInstance = GetModuleHandle(NULL);
    wndClassEx.lpszClassName = Overlay.lpClassName;
    wndClassEx.lpszMenuName = Overlay.lpWindowName;
    wndClassEx.hCursor = NULL;
    wndClassEx.hbrBackground = NULL;
    wndClassEx.hIcon = NULL;
    wndClassEx.hIconSm = NULL;
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbWndExtra = 0;
    wndClassEx.style = 0;

    if (!RegisterClassEx(&wndClassEx)) {
        Logger::WriteLog("Window class registration failed!", Logger::Logger::LogLevel::LOG_ERROR);
        return false;
    }

    if (Game.hWnd) {
        GetClientRect(Game.hWnd, &Game.lpRect);
        ClientToScreen(Game.hWnd, &Game.lpPoint);
    } else {
        Game.lpPoint.x = 0;
        Game.lpPoint.y = 0;
        Game.lpRect.right = GetSystemMetrics(SM_CXSCREEN);
        Game.lpRect.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    Overlay.WindowSize.x = Game.lpRect.right;
    Overlay.WindowSize.y = Game.lpRect.bottom;
    Overlay.hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, Overlay.lpClassName, Overlay.lpWindowName, WS_POPUP | WS_VISIBLE, Game.lpPoint.x, Game.lpPoint.y, Game.lpRect.right - Game.lpRect.left, Game.lpRect.bottom - Game.lpRect.top, NULL, NULL, wndClassEx.hInstance, NULL);

    if (!Overlay.hWnd) {
        Logger::WriteLog("Window creation failed!", Logger::Logger::LogLevel::LOG_ERROR);
        UnregisterClass(Overlay.lpClassName, wndClassEx.hInstance);
        return false;
    }

    if (!SetLayeredWindowAttributes(Overlay.hWnd, RGB(0, 0, 0), 255, LWA_ALPHA)) {
        Logger::WriteLog("Layered window attributes setup failed!", Logger::Logger::LogLevel::LOG_ERROR);
        DestroyWindow(Overlay.hWnd);
        UnregisterClass(Overlay.lpClassName, wndClassEx.hInstance);
        return false;
    }

    MARGINS margin = { -1 };
    if (DwmExtendFrameIntoClientArea(Overlay.hWnd, &margin) != S_OK) {
        Logger::WriteLog("Failed to extend frame into client area!", Logger::Logger::LogLevel::LOG_ERROR);
        DestroyWindow(Overlay.hWnd);
        UnregisterClass(Overlay.lpClassName, wndClassEx.hInstance);
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = Overlay.hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hResult = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &swapChainDesc, &Overlay.pSwapChain, &Overlay.pDevice, &featureLevel, &Overlay.pDeviceContext);
    if (FAILED(hResult)) {
        Logger::WriteLog("Direct3D device and swap chain creation failed!", Logger::Logger::LogLevel::LOG_ERROR);
        DestroyWindow(Overlay.hWnd);
        UnregisterClass(Overlay.lpClassName, wndClassEx.hInstance);
        return false;
    }

    ID3D11Texture2D* pBackBuffer;
    hResult = Overlay.pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hResult)) {
        Logger::WriteLog("Failed to retrieve back buffer!", Logger::Logger::LogLevel::LOG_ERROR);
        Overlay.pSwapChain->Release();
        DestroyWindow(Overlay.hWnd);
        UnregisterClass(Overlay.lpClassName, wndClassEx.hInstance);
        return false;
    }

    hResult = Overlay.pDevice->CreateRenderTargetView(pBackBuffer, NULL, &Overlay.pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hResult)) {
        Logger::WriteLog("Render target view creation failed!", Logger::Logger::LogLevel::LOG_ERROR);
        Overlay.pSwapChain->Release();
        DestroyWindow(Overlay.hWnd);
        UnregisterClass(Overlay.lpClassName, wndClassEx.hInstance);
        return false;
    }

    SetWindowLong(Overlay.hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
    ShowWindow(Overlay.hWnd, SW_SHOWDEFAULT);
    UpdateWindow(Overlay.hWnd);
    return true;
}

void DestroyOverlay() {
    smoke::Cleanup();

    if (Overlay.hWnd) {
        DestroyWindow(Overlay.hWnd);
        Overlay.hWnd = NULL;
    }

    if (Overlay.pRenderTargetView) {
        Overlay.pRenderTargetView->Release();
        Overlay.pRenderTargetView = NULL;
    }

    if (Overlay.pDeviceContext) {
        Overlay.pDeviceContext->Release();
        Overlay.pDeviceContext = NULL;
    }

    if (Overlay.pDevice) {
        Overlay.pDevice->Release();
        Overlay.pDevice = NULL;
    }

    if (Overlay.pSwapChain) {
        Overlay.pSwapChain->Release();
        Overlay.pSwapChain = NULL;
    }

    if (Overlay.lpClassName) {
        UnregisterClass(Overlay.lpClassName, GetModuleHandle(NULL));
        Overlay.lpClassName = NULL;
    }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}