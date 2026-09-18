#pragma once
#include "imgui.h"

namespace font
{
	inline ImFont* icomoon_logo = nullptr;
	inline ImFont* description_font = nullptr;
	inline ImFont* esp_font = nullptr;
	inline ImFont* regular_m = nullptr;
	inline ImFont* regular_l = nullptr;
	inline ImFont* icomoon_page = nullptr;
	inline ImFont* small_font = nullptr;
	inline ImFont* inter_semibold = nullptr;
	inline ImFont* s_inter_semibold = nullptr;
	inline ImFont* inter_medium = nullptr;
	inline ImFont* icon_notify = nullptr;
	inline ImFont* ascii_art = nullptr;
	inline ImFont* tab_font = nullptr;
	inline ImFont* icon_small = nullptr;
}

namespace utils
{
	inline float CalculateTextWidthWithoutColorCodes(const ImFont* font, const char* text_begin, const char* text_end = nullptr) {
		if (!text_end)
			text_end = text_begin + strlen(text_begin);

		float text_width = 0.0f;
		const char* s = text_begin;

		while (s < text_end) {
			if (*s == '^' && (s + 1) < text_end && *(s + 1) >= '0' && *(s + 1) <= '9') {
				s += 2;
				continue;
			}

			unsigned int c = (unsigned int)*s;
			if (c < 0x80) {
				s += 1;
			}
			else {
				s += ImTextCharFromUtf8(&c, s, text_end);
				if (c == 0)
					break;
			}

			const ImFontGlyph* glyph = font->FindGlyph((ImWchar)c);
			if (glyph) {
				text_width += glyph->AdvanceX;
			}
		}

		return text_width;
	}

	inline ImColor GetColorWithAlpha(ImColor color, float alpha)
	{
		return ImColor(color.Value.x, color.Value.y, color.Value.z, alpha);
	}

	inline ImVec2 center_text(ImVec2 min, ImVec2 max, const char* text)
	{
		return min + (max - min) / 2 - ImGui::CalcTextSize(text) / 2;
	}

	inline ImColor GetDarkColor(const ImColor& color)
	{
		float r = color.Value.x;
		float g = color.Value.y;
		float b = color.Value.z;

		float darkPercentage = 0.2f;
		return ImColor(r * darkPercentage, g * darkPercentage, b * darkPercentage, 1.f);
	}

	inline ImVec4 ImColorToImVec4(const ImColor& color)
	{
		return ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w);
	}

	inline ImU32 ToU32(const ImVec4& col) { return ImGui::GetColorU32(col); }
	inline ImU32 ToU32(const ImColor& col) { return ImGui::GetColorU32(ImVec4(col.Value.x, col.Value.y, col.Value.z, col.Value.w)); }
}

namespace Theme
{
	inline ImVec4 Accent         = ImColor(255, 193, 37);
	inline ImVec4 AccentDark     = ImColor(200, 150, 20);
	inline ImVec4 AccentGlow     = ImColor(255, 193, 37, 38);

	inline ImVec4 WindowBg       = ImColor(10, 10, 10);
	inline ImVec4 CardBg         = ImColor(12, 12, 12);
	inline ImVec4 CardHeaderBg   = ImColor(14, 14, 14, 100);
	inline ImVec4 HoverBg        = ImColor(20, 20, 20, 128);
	inline ImVec4 ActiveBg       = ImColor(25, 20, 5, 180);

	inline ImVec4 SidebarBg      = ImColor(10, 10, 10);
	inline ImVec4 SidebarActive  = ImColor(40, 32, 10);
	inline ImVec4 SidebarHover   = ImColor(255, 255, 255, 13);
	inline ImVec4 SidebarBorder  = ImColor(255, 193, 37, 180);

	inline ImVec4 TextPrimary    = ImColor(242, 242, 242);
	inline ImVec4 TextSecondary  = ImColor(138, 138, 144, 180);
	inline ImVec4 TextDim        = ImColor(138, 138, 144, 128);

	inline ImVec4 Separator      = ImColor(42, 42, 46);
	inline ImVec4 Stroke         = ImColor(255, 255, 255, 13);
	inline ImVec4 StrokeLight    = ImColor(255, 255, 255, 20);

	inline ImVec4 ToggleOff      = ImColor(46, 46, 51);
	inline ImVec4 ToggleOn       = ImColor(255, 193, 37, 230);
	inline ImVec4 ToggleCircleOff= ImColor(140, 140, 144);
	inline ImVec4 ToggleCircleOn = ImColor(255, 255, 255);

	inline ImVec4 SliderTrack    = ImColor(46, 46, 51);
	inline ImVec4 SliderFill     = ImColor(255, 193, 37);
	inline ImVec4 SliderHandle   = ImColor(255, 255, 255);

	inline ImVec4 ButtonBg       = ImColor(26, 26, 30);
	inline ImVec4 ButtonHover    = ImColor(255, 193, 37);

	inline float WindowWidth     = 980.f;
	inline float WindowHeight    = 620.f;
	inline float SidebarWidth    = 160.f;
	inline float CornerRadius    = 28.f;
	inline float CardRadius      = 16.f;
	inline float SidebarRadius   = 28.f;
	inline float SidebarItemH    = 44.f;
	inline float ToggleW         = 36.f;
	inline float ToggleH         = 20.f;
	inline float ToggleCircle    = 8.f;
}

inline ImVec4 g_AccentColor = Theme::Accent;

inline namespace c
{
	inline ImColor dark_color(10, 10, 10, 255);
	inline ImColor second_color(10, 10, 10, 255);
	inline ImColor background_color(8, 8, 8, 255);
	inline ImColor bg_main(8, 8, 8, 255);
	inline ImColor card_bg(10, 10, 10, 255);
	inline ImColor card_header_bg(12, 12, 12, 100);
	inline ImColor separator_color(30, 30, 30, 255);
	inline ImColor stroke_color(255, 255, 255, 13);

	inline ImVec4 accent = Theme::Accent;
	inline ImVec4 separator = Theme::Separator;
	inline ImVec4 window_bg_color = Theme::WindowBg;

	inline namespace anim
	{
		inline float speed;
		inline ImColor active(255, 193, 37, 255);
		inline ImColor defaultColor(10, 10, 10, 255);
	}

	inline namespace bg
	{
		inline ImVec4 background = Theme::WindowBg;
		inline ImVec2 size = ImVec2(Theme::WindowWidth, Theme::WindowHeight);
		inline float rounding = Theme::CornerRadius;
	}

	inline namespace sidebar
	{
		inline ImVec4 background = Theme::SidebarBg;
		inline float width = Theme::SidebarWidth;
		inline float rounding = Theme::SidebarRadius;
		inline float item_height = Theme::SidebarItemH;
		inline ImColor text_default(201, 199, 209, 200);
		inline ImColor text_active(255, 193, 37, 255);
		inline ImColor text_hover(201, 199, 209, 255);
		inline ImColor item_active_bg(40, 32, 10);
		inline ImColor item_hover_bg(255, 255, 255, 13);
		inline ImColor active_border(255, 193, 37, 180);
	}

	inline namespace child
	{
		inline ImVec4 background = Theme::CardBg;
		inline ImVec4 stroke = Theme::StrokeLight;
		inline float rounding = Theme::CardRadius;
	}

	namespace page
	{
		inline ImVec4 background_active = Theme::CardBg;
		inline ImVec4 background = Theme::WindowBg;
		inline ImVec4 text_hov = Theme::TextPrimary;
		inline ImVec4 text = Theme::TextPrimary;
		inline float rounding = Theme::CardRadius;
	}

	inline namespace elements
	{
		inline ImVec4 background_hovered = Theme::HoverBg;
		inline ImVec4 background = Theme::CardBg;
		inline float rounding = Theme::CornerRadius;
	}

	inline namespace toggle
	{
		inline ImColor bg_off(46, 46, 51);
		inline ImColor bg_on(255, 193, 37, 230);
		inline ImColor circle_off(140, 140, 144);
		inline ImColor circle_on(255, 255, 255);
		inline float width = Theme::ToggleW;
		inline float height = Theme::ToggleH;
		inline float circle_radius = Theme::ToggleCircle;
	}

	inline namespace checkbox
	{
		inline ImVec4 mark = Theme::Accent;
	}

	inline namespace text
	{
		inline namespace label
		{
			inline ImColor active = ImColor(242, 242, 242, 255);
			inline ImColor hovered = ImColor(220, 220, 220, 255);
			inline ImColor defaultColor = ImColor(200, 200, 200, 255);
		}

		inline namespace description
		{
			inline ImColor active = ImColor(138, 138, 144, 128);
			inline ImColor hovered = ImColor(138, 138, 144, 160);
			inline ImColor defaultColor = ImColor(138, 138, 144, 128);
		}
	}
}
