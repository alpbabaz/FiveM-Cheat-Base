#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include <d3d11.h>

typedef HRESULT(WINAPI* pfnD3DCompile)(
    LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName,
    CONST D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude,
    LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2,
    ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs);

inline pfnD3DCompile g_D3DCompile = nullptr;

namespace smoke {

inline const int TEX_W = 800;
inline const int TEX_H = 600;

inline ID3D11PixelShader*    g_pPS = nullptr;
inline ID3D11VertexShader*   g_pVS = nullptr;
inline ID3D11InputLayout*    g_pLayout = nullptr;
inline ID3D11Buffer*         g_pVB = nullptr;
inline ID3D11Buffer*         g_pCB = nullptr;
inline ID3D11Texture2D*      g_pTex = nullptr;
inline ID3D11RenderTargetView* g_pRTV = nullptr;
inline ID3D11ShaderResourceView* g_pSRV = nullptr;
inline bool                  g_init = false;

struct ShaderCB {
    float phase;
    float texW;
    float texH;
    float _pad;
    float colorR;
    float colorG;
    float colorB;
    float _pad2;
};

inline const char g_hlsl[] = R"(
cbuffer CB : register(b0) {
    float _phase;
    float _texW;
    float _texH;
    float _pad;
    float _colorR;
    float _colorG;
    float _colorB;
    float _pad2;
};

float2 hash22(float2 p) {
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

float gradientNoise(float2 p) {
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(
        lerp(dot(hash22(i + float2(0.0, 0.0)), f - float2(0.0, 0.0)),
             dot(hash22(i + float2(1.0, 0.0)), f - float2(1.0, 0.0)), u.x),
        lerp(dot(hash22(i + float2(0.0, 1.0)), f - float2(0.0, 1.0)),
             dot(hash22(i + float2(1.0, 1.0)), f - float2(1.0, 1.0)), u.x),
        u.y);
}

float fbm(float2 p) {
    float v = 0.0;
    float a = 0.5;
    float2 shift = float2(100.0, 100.0);
    float2 rot = float2(cos(0.5), sin(0.5));
    for (int i = 0; i < 6; i++) {
        v += a * gradientNoise(p);
        p = mul(rot, p) * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target {
    float angle = _phase * 6.2831853;
    float aspect = _texW / _texH;
    float2 suv = float2(uv.x * aspect, uv.y);

    float slow = angle * 0.08;

    float2 q = float2(
        fbm(suv * 1.2 + float2(slow, slow * 0.7)),
        fbm(suv * 1.2 + float2(5.2, 1.3) + float2(slow * 0.6, slow * 1.1))
    );

    float2 r = float2(
        fbm(suv * 1.2 + 3.0 * q + float2(1.7, 9.2) + float2(slow * 0.9, slow * 0.4)),
        fbm(suv * 1.2 + 3.0 * q + float2(8.3, 2.8) + float2(slow * 0.5, slow * 1.3))
    );

    float f = fbm(suv * 1.2 + 3.5 * r);

    float3 baseColor = float3(_colorR, _colorG, _colorB);
    float3 goldDeep   = baseColor * 0.6;
    float3 goldMid    = baseColor;
    float3 goldBright = baseColor * 1.2 + float3(0.1, 0.1, 0.1);

    float v = smoothstep(-0.1, 0.6, f);
    float3 col = lerp(goldDeep, goldMid, saturate(v * 1.5));
    col = lerp(col, goldBright, saturate(v * v * 2.0));

    float density = smoothstep(0.15, 0.65, f + 0.1);

    float2 centered = uv - 0.5;

    float edgeDistX = min(uv.x, 1.0 - uv.x) * 2.0;
    float edgeDistY = min(uv.y, 1.0 - uv.y) * 2.0;
    float edgeDist = min(edgeDistX, edgeDistY);
    float edgeMask = 1.0 - smoothstep(0.0, 0.6, edgeDist);
    edgeMask = pow(edgeMask, 0.8);

    density *= edgeMask;

    float edgeX = smoothstep(0.0, 0.02, uv.x) * smoothstep(1.0, 0.98, uv.x);
    float edgeY = smoothstep(0.0, 0.04, uv.y) * smoothstep(1.0, 0.96, uv.y);
    density *= edgeX * edgeY;

    float alpha = saturate(density * 0.9);
    if (alpha < 0.003) discard;

    return float4(col, alpha);
}
)";

struct VSInput {
    float pos[2];
    float uv[2];
};

inline const VSInput g_quad[4] = {
    { {-1.0f,  1.0f}, {0.0f, 0.0f} },
    { { 1.0f,  1.0f}, {1.0f, 0.0f} },
    { {-1.0f, -1.0f}, {0.0f, 1.0f} },
    { { 1.0f, -1.0f}, {1.0f, 1.0f} },
};

inline const char g_vs_src[] = R"(
struct VSOut {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};
VSOut main(float2 pos : POSITION, float2 uv : TEXCOORD0) {
    VSOut o;
    o.pos = float4(pos, 0, 1);
    o.uv = uv;
    return o;
}
)";

inline void SafeRelease(IUnknown*& p) {
    if (p) { p->Release(); p = nullptr; }
}

inline bool Init(ID3D11Device* device) {
    if (g_init) return true;
    if (!device) return false;

    if (!g_D3DCompile) {
        HMODULE hMod = LoadLibraryA("d3dcompiler_47.dll");
        if (!hMod) hMod = LoadLibraryA("d3dcompiler.dll");
        if (hMod) g_D3DCompile = (pfnD3DCompile)GetProcAddress(hMod, "D3DCompile");
        if (!g_D3DCompile) return false;
    }

    HRESULT hr;
    ID3DBlob* blob = nullptr;
    ID3DBlob* errBlob = nullptr;

    hr = g_D3DCompile(g_vs_src, sizeof(g_vs_src) - 1, "vs_main", nullptr, nullptr,
        "main", "vs_5_0", 0, 0, &blob, &errBlob);
    if (FAILED(hr)) {
        if (errBlob) { errBlob->Release(); }
        return false;
    }
    hr = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(),
        nullptr, &g_pVS);
    if (FAILED(hr)) { blob->Release(); return false; }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    hr = device->CreateInputLayout(layout, 2, blob->GetBufferPointer(),
        blob->GetBufferSize(), &g_pLayout);
    blob->Release();
    if (FAILED(hr)) return false;

    hr = g_D3DCompile(g_hlsl, sizeof(g_hlsl) - 1, "ps_main", nullptr, nullptr,
        "main", "ps_5_0", 0, 0, &blob, &errBlob);
    if (FAILED(hr)) {
        if (errBlob) { errBlob->Release(); }
        return false;
    }
    hr = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(),
        nullptr, &g_pPS);
    blob->Release();
    if (FAILED(hr)) return false;

    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(VSInput) * 4;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init = {};
    init.pSysMem = g_quad;
    hr = device->CreateBuffer(&bd, &init, &g_pVB);
    if (FAILED(hr)) return false;

    bd.ByteWidth = ((sizeof(ShaderCB) + 15) / 16) * 16;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&bd, nullptr, &g_pCB);
    if (FAILED(hr)) return false;

    D3D11_TEXTURE2D_DESC td = {};
    td.Width = TEX_W;
    td.Height = TEX_H;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = device->CreateTexture2D(&td, nullptr, &g_pTex);
    if (FAILED(hr)) return false;

    hr = device->CreateRenderTargetView(g_pTex, nullptr, &g_pRTV);
    if (FAILED(hr)) return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC sd = {};
    sd.Format = td.Format;
    sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    sd.Texture2D.MipLevels = 1;
    hr = device->CreateShaderResourceView(g_pTex, &sd, &g_pSRV);
    if (FAILED(hr)) return false;

    g_init = true;
    return true;
}

inline void Render(ID3D11Device* device, ID3D11DeviceContext* ctx, float time) {
    if (!g_init) {
        if (!Init(device)) return;
    }
    if (!ctx) return;

    float phase = fmodf(time, 8.0f) / 8.0f;

    ShaderCB cb;
    cb.phase = phase;
    cb.texW = (float)TEX_W;
    cb.texH = (float)TEX_H;
    cb._pad = 0.0f;
    cb.colorR = Theme::Accent.x;
    cb.colorG = Theme::Accent.y;
    cb.colorB = Theme::Accent.z;
    cb._pad2 = 0.0f;
    ctx->UpdateSubresource(g_pCB, 0, nullptr, &cb, 0, 0);

    ID3D11RenderTargetView* oldRT = nullptr;
    ID3D11DepthStencilView* oldDS = nullptr;
    ctx->OMGetRenderTargets(1, &oldRT, &oldDS);

    D3D11_VIEWPORT oldVP;
    UINT numVPs = 1;
    ctx->RSGetViewports(&numVPs, &oldVP);

    float clearCol[4] = { 0, 0, 0, 0 };
    ctx->ClearRenderTargetView(g_pRTV, clearCol);

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)TEX_W;
    vp.Height = (float)TEX_H;
    vp.MaxDepth = 1.0f;
    ctx->RSSetViewports(1, &vp);
    ctx->OMSetRenderTargets(1, &g_pRTV, nullptr);

    UINT stride = sizeof(VSInput);
    UINT offset = 0;
    ctx->IASetInputLayout(g_pLayout);
    ctx->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    ctx->VSSetShader(g_pVS, nullptr, 0);
    ctx->PSSetShader(g_pPS, nullptr, 0);
    ctx->PSSetConstantBuffers(0, 1, &g_pCB);

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_NONE;
    ID3D11RasterizerState* rsState = nullptr;
    device->CreateRasterizerState(&rsDesc, &rsState);
    ctx->RSSetState(rsState);

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = FALSE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    ID3D11BlendState* bsState = nullptr;
    device->CreateBlendState(&blendDesc, &bsState);
    float blendFactor[4] = { 0, 0, 0, 0 };
    ctx->OMSetBlendState(bsState, blendFactor, 0xFFFFFFFF);

    ctx->Draw(4, 0);

    ctx->OMSetRenderTargets(1, &oldRT, oldDS);
    ctx->RSSetViewports(1, &oldVP);

    if (oldRT) oldRT->Release();
    if (oldDS) oldDS->Release();
    if (rsState) rsState->Release();
    if (bsState) bsState->Release();
}

inline ID3D11ShaderResourceView* GetTexture() {
    return g_pSRV;
}

inline void Cleanup() {
    SafeRelease((IUnknown*&)g_pSRV);
    SafeRelease((IUnknown*&)g_pRTV);
    SafeRelease((IUnknown*&)g_pTex);
    SafeRelease((IUnknown*&)g_pCB);
    SafeRelease((IUnknown*&)g_pVB);
    SafeRelease((IUnknown*&)g_pLayout);
    SafeRelease((IUnknown*&)g_pPS);
    SafeRelease((IUnknown*&)g_pVS);
    g_init = false;
}

}
