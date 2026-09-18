#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "images.h"
#include "notifications.h"
#include "font_defines.h"
#include "directx_blur.h"
#include "logo_png.h"
#include <random>
#include "custom_widgets.hpp"
#include <DirectX\D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#include <d3d11.h>
#include <tchar.h>

HWND hwnd;
RECT rc;

#include "font.h"
#include "image.h"
#include "esp.h"

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

bool dark = true;

char field[45] = { "" };

int page = 0;

static float tab_alpha = 0.f; /* */ static float tab_add; /* */ static int active_tab = 0;

int key, m;

bool checkbox_on = true;
bool checkbox_off = false;

static float slider_float = 0.5f;

static int select1 = 0;
const char* items[3]{ "Selected", "Nope", "what :/ a y?" };

static int bullet_select = 0;
const char* bullet[2]{ "Disabled", "Enabled" };

static int sound_select = 0;
const char* sound[2]{ "Disabled", "Enabled" };

static int style_select = 0;
const char* stylee[2]{ "Flat", "Back" };

static int style2_select = 0;
const char* stylee2[2]{ "Textured", "3D Mode" };

static bool multi_num[5] = { false, true, true, true, false };
const char* multi_items[5] = { "One", "Two", "Three", "Four", "Five" };

float knob = 1.f;

float col[4] = { anim::active.Value.x, anim::active.Value.y, anim::active.Value.z, 0.5f };


static bool tab_is_changed = false;
static float tab_offset = 0.f;

int rotation_start_index;
void ImRotateStart()
{
	rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImRotationCenter()
{
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

	const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}

void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
	float s = sin(rad), c = cos(rad);
	center = ImRotate(center, s, c) - center;

	auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}


void move_window() {
	ImGui::SetCursorPos(ImVec2(0, 0));
	if (ImGui::InvisibleButton("Move_detector", c::bg::size));
	if (ImGui::IsItemActive()) {
		GetWindowRect(hwnd, &rc);
		MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, c::bg::size.x, c::bg::size.y, TRUE);
	}
}

void RenderBlur(HWND hwnd)
{
	struct ACCENTPOLICY
	{
		int na;
		int nf;
		int nc;
		int nA;
	};
	struct WINCOMPATTRDATA
	{
		int na;
		PVOID pd;
		ULONG ul;
	};

	const HINSTANCE hm = LoadLibrary("user32.dll");
	if (hm)
	{
		typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

		const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hm, "SetWindowCompositionAttribute");
		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { 3, 0, 0, 0 };

			WINCOMPATTRDATA data = { 19, &policy,sizeof(ACCENTPOLICY) };
			SetWindowCompositionAttribute(hwnd, &data);
		}
		FreeLibrary(hm);
	}
}

struct s_tab
{
	std::vector<const char*> icon;
	std::vector<const char*> name;
};

static class c_tabs {

private:

	int current_idx;
	int check_id = 0;

	bool want_to_change;

	int wanted_idx;

	std::vector<s_tab> tab_selection;



public:

	c_tabs(std::vector<s_tab> tab_info)
	{
		this->current_idx = 0;
		this->check_id = 0;
		this->wanted_idx = 0;
		this->want_to_change = true;
		this->tab_selection = tab_info;
	}


	int GetCurrentTab()
	{
		return current_idx;
	}


	bool IsTabActive(int id)
	{
		return bool(this->c_tabs::current_idx == id);
	}

	void UpdateTabs(std::vector<s_tab> tab_info)
	{
		this->tab_selection = tab_info;
	}


	 
	void DrawTabs()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 10));
		ImGui::SetCursorPos(ImVec2(10.f, 135));
		ImGui::BeginChild("Tabs", ImVec2(c::sidebar::width - 20, ImGui::GetContentRegionAvail().y - 10), false, ImGuiWindowFlags_NoScrollbar);
		{
			this->check_id = 0;
			for (int n = 0; n < tab_selection.size(); n++) {
				for (int i = 0; i < tab_selection[n].icon.size(); i++)
				{
					if (custom::Tab(tab_selection[n].name[i], tab_selection[n].icon[i], &this->c_tabs::current_idx, int(this->check_id))) {
						tab_is_changed = true;
						want_to_change = true;
						wanted_idx = int(this->check_id);
					}
					this->check_id++;
				}
			}

			if (want_to_change) {
				if (!tab_is_changed) {
					this->c_tabs::current_idx = wanted_idx;
					want_to_change = false;
				}
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

};

static struct c_animated_bg
{

private:

	const int p_cout = 60;

	ImVec2 GenerateRandomPositionInRect(const ImRect& rect) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> x_dist(rect.Min.x, rect.Max.x);
		std::uniform_real_distribution<float> y_dist(rect.Min.y, rect.Max.y);

		float random_x = x_dist(gen);
		float random_y = y_dist(gen);

		return ImVec2(random_x, random_y);
	}

	float GenerateRandomFloat(float min, float max) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(min, max);

		return dist(gen);
	}

	struct p_info
	{
		ImVec2 current_pos;
		ImVec2 target_pos;
		float radius, current_angle, target_angle, speed, alpha, anim_time, timer;
	};

	std::vector<p_info> particle;

	float Distance(const ImVec2& point1, const ImVec2& point2) {
		float deltaX = point2.x - point1.x;
		float deltaY = point2.y - point1.y;

		return std::sqrt(deltaX * deltaX + deltaY * deltaY);
	}

	inline ImVec2 vector_angles(const ImVec2& start, const ImVec2& end)
	{
		const ImVec2 delta = end - start;

		return ImVec2(std::atan2(delta.y, delta.x),
			std::atan2(std::sqrt(delta.x * delta.x + delta.y * delta.y),
				0.0f));
	}

	void rotate_to(int idx, ImVec2 vec)
	{
		const auto local_pos = this->particle[idx].current_pos;

		auto delta_angles = vector_angles(ImGui::GetMousePos(), local_pos);
		delta_angles.x += 0.1f;

		this->particle[idx].current_angle = delta_angles.x;
	}

public:

	c_animated_bg()
	{
		particle.resize(p_cout);
	}

	void render()
	{
		for (int i = 0; i < p_cout; i++)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			if (!window->Rect().Contains(particle[i].target_pos))
			{
				particle[i].target_pos = GenerateRandomPositionInRect(window->Rect());
				particle[i].speed = 0.6f + GenerateRandomFloat(0.5f, 1.0f);
				particle[i].current_angle = GenerateRandomFloat(0.5f, 360.f);
				particle[i].target_angle = GenerateRandomFloat(0.5f, 360.f);
				particle[i].radius = GenerateRandomFloat(1.5f, 3.5f);
				particle[i].anim_time = GenerateRandomFloat(1500, 4000);
				particle[i].timer = 0.f;
				particle[i].alpha = -3.f;
			}


			particle[i].alpha = ImLerp(particle[i].alpha, 1.f, c::anim::speed / 10);



			particle[i].timer += c::anim::speed;

			if (particle[i].timer < particle[i].anim_time / 2)
				particle[i].target_angle = 180;

			if (particle[i].timer > particle[i].anim_time / 2)
				particle[i].target_angle = 0;


			if (particle[i].timer > particle[i].anim_time)
			{
				particle[i].target_angle = 360;
				particle[i].timer = 0;
			}

			particle[i].target_pos.x += cosf(particle[i].current_angle) * particle[i].speed;
			particle[i].target_pos.y += sinf(particle[i].current_angle) * particle[i].speed;



			particle[i].current_pos = ImLerp(particle[i].current_pos, particle[i].target_pos, c::anim::speed);


			window->DrawList->AddShadowCircle(particle[i].current_pos, particle[i].radius, utils::GetColorWithAlpha(c::anim::active, particle[i].alpha), 45.f, ImVec2(0, 0), 0, 36);
			window->DrawList->AddCircleFilled(particle[i].current_pos, particle[i].radius + 2.5f, utils::GetColorWithAlpha(c::anim::active, particle[i].alpha / 2), 36);
			window->DrawList->AddCircleFilled(particle[i].current_pos, particle[i].radius, utils::GetColorWithAlpha(c::anim::active, particle[i].alpha), 36);
		}
	}

};

namespace texture
{
	ID3D11ShaderResourceView* preview_slow = nullptr;
	ID3D11ShaderResourceView* window_bg = nullptr;
	ID3D11ShaderResourceView* model_image = nullptr;
	ID3D11ShaderResourceView* menu_bg = nullptr;
	ID3D11ShaderResourceView* avatar_image = nullptr;
	ID3D11ShaderResourceView* logotype_image = nullptr;
	ID3D11ShaderResourceView* anim_logo = nullptr;
	ID3D11ShaderResourceView* logo_image = nullptr;
}


static bool image_loaded = false;

static class c_welcome_bg
{

private:

	float welcome_progress;
	float cheatname_progress;
	float logo_progress;
	float background_alpha;

	bool is_finish = false;

	const char* top_text;
	const char* bottom_text;

public:

	c_welcome_bg(const char* welcome_text, const char* cheat_name)
	{
		top_text = welcome_text;
		bottom_text = cheat_name;
		cheatname_progress = 0.f;
		logo_progress = 0.f;
		welcome_progress = 0.f;
		background_alpha = 0.f;
	}

	void update()
	{
		static DWORD dwTickStart = GetTickCount();
		if (GetTickCount() - dwTickStart > 150)
		{

			if (!this->is_finish) {
				this->c_welcome_bg::welcome_progress = ImLerp(this->c_welcome_bg::welcome_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);

				if (GetTickCount() - dwTickStart > 1000)
					this->c_welcome_bg::cheatname_progress = ImLerp(this->c_welcome_bg::cheatname_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);

				if (GetTickCount() - dwTickStart > 1200)
					this->c_welcome_bg::logo_progress = ImLerp(this->c_welcome_bg::logo_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);

				this->c_welcome_bg::background_alpha = ImLerp(this->c_welcome_bg::background_alpha, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);
			}

			else {

				if (GetTickCount() - dwTickStart > 3000)
					this->c_welcome_bg::welcome_progress = ImLerp(this->c_welcome_bg::welcome_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);

				if (GetTickCount() - dwTickStart > 3200)
					this->c_welcome_bg::logo_progress = ImLerp(this->c_welcome_bg::logo_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);

				if (GetTickCount() - dwTickStart > 2600)
					this->c_welcome_bg::cheatname_progress = ImLerp(this->c_welcome_bg::cheatname_progress, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);

				if (GetTickCount() - dwTickStart > 3400)
					this->c_welcome_bg::background_alpha = ImLerp(this->c_welcome_bg::background_alpha, this->is_finish ? 0.f : 1.f, c::anim::speed / 3);
			}
			if (GetTickCount() - dwTickStart > 2000)
				this->is_finish = true;

		}

		ImRect window_bb = ImGui::GetCurrentWindow()->Rect();

		ImGui::GetForegroundDrawList()->AddRectFilled(window_bb.Min, window_bb.Max, utils::GetColorWithAlpha(c::window_bg_color, this->c_welcome_bg::background_alpha), ImGui::GetStyle().FrameRounding);

		ImGui::PushFont(font::regular_m);
		ImGui::GetForegroundDrawList()->AddText(ImVec2(utils::center_text(window_bb.Min, window_bb.Max, bottom_text).x, window_bb.GetCenter().y -
			(-20 * this->c_welcome_bg::cheatname_progress)), utils::GetColorWithAlpha(c::anim::active,
				this->c_welcome_bg::cheatname_progress), bottom_text);


		ImGui::PopFont();

		ImGui::PushFont(font::regular_l);
		ImGui::GetForegroundDrawList()->AddText(ImVec2(utils::center_text(window_bb.Min, window_bb.Max, top_text).x, window_bb.GetCenter().y - (50 * this->c_welcome_bg::welcome_progress)), utils::GetColorWithAlpha(c::label::active, this->c_welcome_bg::welcome_progress), top_text);
		ImGui::PopFont();

		static int current_frame = 0;

		static float frame_offset = 0.f;
		static float static_frame_offset = 0.01111111111f;

		frame_offset = current_frame * static_frame_offset;

		static DWORD dwTickStart2 = GetTickCount();
		if (GetTickCount() - dwTickStart2 > 15)
		{
			if (current_frame + 1 <= 90)
				current_frame++;
			else
				current_frame = 0;

			dwTickStart2 = GetTickCount();
		}

		ImRect image_bb(window_bb.GetCenter() - ImVec2(30, 30), window_bb.GetCenter() + ImVec2(30, 30));

		ImGui::GetForegroundDrawList()->AddImageRounded(texture::logotype_image, image_bb.Min - ImVec2(0, 105.f * this->c_welcome_bg::logo_progress), image_bb.Max - ImVec2(0, 105.f * this->c_welcome_bg::logo_progress), ImVec2(0 + frame_offset, 0), ImVec2(frame_offset + static_frame_offset, 1), ImColor(1.f, 1.f, 1.f, this->c_welcome_bg::logo_progress), c::bg::rounding);


	}

	bool is_finished()
	{
		return is_finish;
	};



};

void LoadImages() {
	if (image_loaded)
		return;

	D3DX11_IMAGE_LOAD_INFO iInfo;
	ID3DX11ThreadPump* threadPump{ nullptr };
	
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, logotype, sizeof(logotype), &iInfo, threadPump, &texture::logotype_image, 0);
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, bg_pic_bytes, sizeof(bg_pic_bytes), &iInfo, threadPump, &texture::window_bg, 0);
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, model_bytes, sizeof(model_bytes), &iInfo, threadPump, &texture::model_image, 0);
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, menu_background, sizeof(menu_background), &iInfo, threadPump, &texture::menu_bg, 0);
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, animated_logotype, sizeof(animated_logotype), &iInfo, threadPump, &texture::anim_logo, 0);
	D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, window_bg_bytes, sizeof(window_bg_bytes), &iInfo, threadPump, &texture::window_bg, 0);

	

	image_loaded = true;
}