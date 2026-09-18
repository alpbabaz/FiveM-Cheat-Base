#include "AmGui/main.h"
#include "../AmGui/smoke_effect.h"

void initializeMenu() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	{
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImFontConfig cfg;
		cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;;

		io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		static ImWchar icomoon_ranges[] = { 0x1, 0x10FFFD, 0 };

		static ImFontConfig icomoon_config;
		icomoon_config.OversampleH = icomoon_config.OversampleV = 1;
		icomoon_config.MergeMode = true;
		icomoon_config.GlyphOffset.y = 6.5f;
		icomoon_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 25.f, &icomoon_config, icomoon_ranges);

		font::esp_font = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::description_font = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::regular_m = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 21.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::regular_l = io.Fonts->AddFontFromMemoryTTF(SFProDisplayRegular, sizeof(SFProDisplayRegular), 41.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::s_inter_semibold = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::inter_semibold = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 29.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::small_font = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 14.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::inter_medium = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::icomoon_page = io.Fonts->AddFontFromMemoryTTF(icomoon_page, sizeof(icomoon_page), 28.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::icomoon_logo = io.Fonts->AddFontFromMemoryTTF(icomoon_page, sizeof(icomoon_page), 30.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::icon_notify = io.Fonts->AddFontFromMemoryTTF(icon_notify, sizeof(icon_notify), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
		font::tab_font = io.Fonts->AddFontFromMemoryTTF(SFProDisplayMedium, sizeof(SFProDisplayMedium), 16.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

		static ImWchar icon_full_range[] = { 0x1, 0x10FFFD, 0 };
		ImFontConfig icon_cfg;
		icon_cfg.MergeMode = false;
		font::icon_small = io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 18.f, &icon_cfg, icon_full_range);

		static ImWchar art_ranges[] = { 0x0020, 0x00FF, 0x2580, 0x259F, 0 };
		ImFontConfig art_cfg;
		art_cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting | ImGuiFreeTypeBuilderFlags_LightHinting;
		font::ascii_art = io.Fonts->AddFontFromMemoryTTF(inter_semibold, sizeof(inter_semibold), 6.f, &art_cfg, art_ranges);
	}
}

namespace Cheats {
	int menuKey = VK_F9;

	namespace Settings {
		bool vsync = true;
		bool streamproof = true;
		bool watermark = true;
		int selectedColor = 0; 
	}

	namespace Auth {
		inline std::string keyName = "N/A";
		inline std::string keyDate = "N/A";
		inline std::string username = "N/A";
		inline bool authenticated = false;
	}
	namespace Esp {
		bool skeletonEsp = false;
		float skeletonEspColor[3] = { 1.0f, 1.0f, 1.0f };
		int skeletonEspSize = 1;
		bool boxEsp = false;
		float boxEspColor[3] = { 1.0f, 1.0f, 1.0f };
		bool useCustomGradient = false;
		float boxGradientTopColor[4] = { 1.0f, 1.0f, 1.0f, 0.25f };
		float boxGradientBottomColor[4] = { 0.1f, 0.1f, 0.1f, 0.35f };
		bool gradientEnabled = false;
		float gradientIntensity = 1.0f;
		int boxEspSelectedType = 0;
		const char* boxEspTypes[2]{ "2D", "Corner" };
		float BoxSizeX = 0.75f;
		bool lineEsp = false;
		float lineEspColor[3] = { 1.0f, 1.0f, 1.0f };
		int lineEspSelectedType = 0;
		const char* lineEspTypes[3]{ "Top", "Center", "Bottom" };
		bool nameEsp = false;
		float nameEspColor[3] = { 1.0f, 1.0f, 1.0f };
		bool weaponEsp = false;
		float weaponEspColor[3] = { 1.0f, 1.0f, 1.0f };
		bool distanceEsp = false;
		float distanceEspColor[3] = { 1.0f, 1.0f, 1.0f };
		bool directionEsp = false;
		float directionEspColor[3] = { 1.0f, 1.0f, 1.0f };
		bool healthBar = false;
		bool armorBar = false;
		bool radarEsp = false;
		bool onlyVisible = false;
		bool ignorePed = false;
		bool ignoreDeath = false;
		int distance = 200;
	}

	namespace Vehicle {
		bool vehicleEsp = false;
		bool drawLocalVehicle = false;
		bool drawEnemyVehicle = false;
		bool vehicleHealt = false;
		bool vehicleEspShowDistance = false;
		bool vehicleEspSnapline = false;
		bool vehicleMarker = false;
		int distance = 200;
	};

	namespace Aim {
		bool aimBot = false;
		int aimbotSelectedType = 0;
		const char* aimbotTypes[3]{ "Head", "Body", "Neck" };
		int aimBotFov = 150;
		int aimBotSmooth = 5;
		int aimBotKey;
		bool silent = false;
		int SlientSlectedType = 0;
		const char* SlientTypes[4]{ "Head", "Body", "Random", "Neck" };
		int silentFov = 150;
		int silentKey;
		bool Pslient = false;
		bool crossHair = false;
		int crossHairSelectedType = 0;
		const char* crossHairTypes[10]{ "Type 1", "Type 2", "Type 3", "Type 4", "Type 5", "Type 6", "Type 7", "Type 8", "Type 9", "Type 10" };
		int crossHairSize = 10;
		float crossHairColor[3] = { 1.0f, 1.0f, 1.0f };
		bool drawAimbotFov = false;
		float drawAimbotFovColor[3] = { 1.0f, 1.0f, 1.0f };
		bool drawSilentFov = false;
		float drawSilentFovColor[3] = { 1.0f, 1.0f, 1.0f };
		bool ignorePed = false;
		bool ignoreDeath = false;
		int distance = 500;
		bool triggerBot = false;
		int triggerFov = 100;
		int triggerDelay = 1;
		int triggerKey;
		int crosshairTolerance = 5;
		const char* triggerBotTypes[3]{ "Head", "Body", "Neck" };
		int triggerBotSelectedType = 0;
		bool drawTriggerFov = false;
		float drawTriggerFovColor[3] = { 1.0f, 1.0f, 1.0f };
		bool onlyVisible = false;
	}

	namespace Misc {
		bool infiniteAmmo = false;
		bool noRecoil = false;
		bool noSpread = false;
		bool noReload = false;
		bool noRange = false;
		bool damageBoost = false;
		int damageValue = 200;
		bool reloadAmmo = false;
		int reloadValue = 1;
		int reloadAmmoKey;
		bool TeleportWaypoint = false;
		bool NoClip = false;
		int NoClipSpeed;
		int NoClipKey;
		bool godMode = false;
		int godModeKey;
		bool healthBoost = false;
		float healthBoostValue = 200.0f;
		int healthBoostKey;
		bool armorBoost = false;
		int armorBoostValue = 50;
		int armorBoostKey;
		bool fovChanger = false;
		int fovValue = 50;
		bool vehicleFix = false;
		bool vehicleUnlock = false;
	}
}

static bool checkboxes[60];
static int slider_int[30];
float color_edit[10][4];
static int combo[30];
static int keybind[30];
static int keybind_mode[30];
const char* combo_list[] = { "#1", "#2", "#3", "#4", "#5" };
static int iTabs;
static int iSubTabs;
static int aimSubTab = 0;
static int prevTab = 0;
static float tabSlideOffset = 0.f;

static float menu_alpha = 1.F;
static bool menu_active = true;

static char PassWord[50] = { "" };
static char Licence[50] = { "" };
static char UserName[50] = { "" };
static char RgPassWord[50] = { "" };
static char RgUserName[50] = { "" };

void UpdateFloatWithLerp(bool& condition, float& a, float b, float c) {
	float deltaTime = ImGui::GetIO().DeltaTime;
	float speed = 11.0f;
	if (condition) {
		a = ImLerp(a, c, deltaTime * speed);
		if (ImAbs(a - c) < 30.f) {
			condition = false;
		}
	}
	else {
		a = ImLerp(a, b, deltaTime * speed);
	}
}

static constexpr float CARD_HEADER_H = 52.f;

void DrawCardBg(const char* title, const char* desc, ImVec2 pos, ImVec2 size, ImGuiID id) {
	ImDrawList* dl = ImGui::GetWindowDrawList();

	ImU32 bgCol = utils::ToU32(Theme::CardBg);
	ImU32 borderCol = utils::ToU32(Theme::StrokeLight);
	ImU32 headerBgCol = utils::ToU32(Theme::CardHeaderBg);
	ImU32 accentCol = utils::ToU32(Theme::Accent);
	ImU32 textCol = utils::ToU32(Theme::TextPrimary);
	ImU32 descCol = utils::ToU32(Theme::TextDim);
	ImU32 sepCol = utils::ToU32(Theme::Separator);

	dl->AddRectFilled(pos, pos + size, bgCol, Theme::CardRadius);
	dl->AddRectFilled(pos, pos + ImVec2(size.x, CARD_HEADER_H), headerBgCol, Theme::CardRadius, ImDrawFlags_RoundCornersTop);
	dl->AddRectFilled(pos + ImVec2(12, 0), pos + ImVec2(42, 2), accentCol, 1.f);
	dl->AddRect(pos, pos + size, borderCol, Theme::CardRadius);

	ImGui::PushFont(font::s_inter_semibold);
	dl->AddText(pos + ImVec2(14, 14), textCol, title);
	ImGui::PopFont();

	dl->AddText(pos + ImVec2(14, 32), descCol, desc);

	dl->AddRectFilled(pos + ImVec2(12, CARD_HEADER_H - 1), pos + ImVec2(size.x - 12, CARD_HEADER_H), sepCol);
}

void BeginCardContent(ImVec2 cardPos, ImVec2 cardSize, const char* childId) {
	ImGui::SetCursorScreenPos(cardPos + ImVec2(0, CARD_HEADER_H));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 10));
	ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);
	ImGui::BeginChild(childId, cardSize - ImVec2(0, CARD_HEADER_H), false,
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);
}

void EndCardContent() {
	ImGui::EndChild();
	ImGui::PopStyleVar(3);
}

void drawMenu() {
	ImGuiStyle& s = ImGui::GetStyle();
	s.FramePadding = ImVec2(12, 8);
	s.ItemSpacing = ImVec2(12, 6);
	s.FrameRounding = 14.f;
	s.WindowRounding = 28.f;
	s.WindowBorderSize = 0.f;
	s.PopupBorderSize = 0.f;
	s.WindowPadding = ImVec2(0, 0);
	s.ChildBorderSize = 0.f;
	s.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
	s.Colors[ImGuiCol_Separator] = ImVec4(1.f, 1.f, 1.f, 0.08f);
	s.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 0.f);
	s.Colors[ImGuiCol_ChildBg] = ImVec4(0.f, 0.f, 0.f, 0.f);
	s.WindowShadowSize = 0;
	s.PopupRounding = 14.f;
	s.ScrollbarSize = 4;
	s.SeparatorTextPadding = ImVec2(10, 10);

	static std::vector<s_tab> tabs_info;
	if (tabs_info.empty()) {
		tabs_info.push_back({ {ICON_AIMING_2_LINE, ICON_GROUP_3_LINE, ICON_BOMB_LINE, ICON_SETTINGS_1_FILL },
			{"Aim", "Visual", "Misc", "Settings"} });
	}

	static c_tabs p_tabs(tabs_info);
	static c_animated_bg p_animated_bg;
	LoadImages();

	UpdateFloatWithLerp(tab_is_changed, tab_offset, 0.f, 600.f);

	int currentTab = -1;
	for (int i = 0; i < 4; i++) { if (p_tabs.IsTabActive(i)) { currentTab = i; break; } }
	if (currentTab >= 0 && currentTab != prevTab) {
		float dir = (currentTab > prevTab) ? 1.f : -1.f;
		tabSlideOffset = dir * 80.f;
		prevTab = currentTab;
	}
	float dt = ImGui::GetIO().DeltaTime;
	tabSlideOffset = ImLerp(tabSlideOffset, 0.f, dt * 12.f);
	float tabAlpha = 1.f - ImClamp(fabsf(tabSlideOffset) / 80.f, 0.f, 1.f);

	ImGui::SetNextWindowSize(c::bg::size);
	Begin("CFXSENSE", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
	{
		c::anim::speed = ImGui::GetIO().DeltaTime * 12.f;
		const ImVec2& pos = ImGui::GetWindowPos();
		const ImVec2& region = ImGui::GetContentRegionMax();
		const ImVec2& spacing = s.ItemSpacing;
		menu_alpha = ImLerp(menu_alpha, menu_active ? 1.f : 0.f, c::anim::speed);
		s.Alpha = menu_alpha;
		handle_input();

		ImDrawList* dl = GetWindowDrawList();

		dl->AddShadowRect(pos, pos + c::bg::size, ImColor((int)(Theme::Accent.x * 255), (int)(Theme::Accent.y * 255), (int)(Theme::Accent.z * 255), 15), 25.f, ImVec2(0, 0), ImDrawFlags_RoundCornersAll, c::bg::rounding);

		dl->AddRectFilled(pos, pos + c::bg::size, ImGui::GetColorU32(ImVec4(0.01f, 0.01f, 0.01f, 1.0f)), c::bg::rounding);

		if (smoke::GetTexture()) {
			dl->AddImageRounded((ImTextureID)smoke::GetTexture(), pos, pos + c::bg::size, ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), c::bg::rounding);
		}

		float sidebar_w = c::sidebar::width;
		float sidebar_h = c::bg::size.y;

		dl->AddRectFilled(pos, pos + ImVec2(sidebar_w, sidebar_h), ImGui::GetColorU32(ImVec4(0.01f, 0.01f, 0.01f, 1.0f)), c::bg::rounding, ImDrawFlags_RoundCornersLeft);
		dl->AddRectFilled(pos + ImVec2(sidebar_w, 0), pos + ImVec2(sidebar_w + 1, sidebar_h), ImGui::GetColorU32(ImVec4(0.06f, 0.06f, 0.06f, 1.0f)));


		dl->AddRect(pos, pos + c::bg::size, c::stroke_color, c::bg::rounding);

		if (texture::logo_image) {
			float logo_size = 90.f;
			ImVec2 logoPos = pos + ImVec2((sidebar_w - logo_size) * 0.5f, 12);
			dl->AddImage((ImTextureID)texture::logo_image, logoPos, logoPos + ImVec2(logo_size, logo_size));
		}

		p_tabs.DrawTabs();

		{
			float f9_y = pos.y + sidebar_h - 44.f;
			float f9_center_x = pos.x + sidebar_w * 0.5f;

			ImU32 keyBg = ImGui::GetColorU32(ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
			ImU32 keyBorder = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImU32 keyText = ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 1.0f));
			ImU32 keySub = ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.7f));

			ImVec2 kbMin(f9_center_x - 32.f, f9_y);
			ImVec2 kbMax(f9_center_x + 32.f, f9_y + 32.f);
			dl->AddRectFilled(kbMin, kbMax, keyBg, 6.f);
			dl->AddRect(kbMin, kbMax, keyBorder, 6.f);

			ImGui::PushFont(font::small_font);
			dl->AddText(ImVec2(f9_center_x - 24.f, f9_y + 5.f), keyText, ICON_KEYBOARD_2_FILL);
			dl->AddText(ImVec2(f9_center_x + 4.f, f9_y + 5.f), keySub, "F9");
			ImGui::PopFont();
		}

		float content_x = sidebar_w + 16.f;
		float content_y = 40.f;
		float content_w = c::bg::size.x - sidebar_w - 32.f;
		float content_h = c::bg::size.y - 72.f;
		float half_w = content_w / 2 - 6;

		{
			float btn_size = 30.f;
			float btn_gap = 6.f;
			float btn_y_abs = pos.y + 8.f;
			float btn_x_right = pos.x + c::bg::size.x - 10.f;

			float eye_x = btn_x_right - btn_size;
			float power_x = eye_x - btn_gap - btn_size;

			ImVec2 mpos = ImGui::GetIO().MousePos;

			ImVec2 eyeMin(eye_x, btn_y_abs);
			ImVec2 eyeMax(eye_x + btn_size, btn_y_abs + btn_size);
			bool eyeHovered = (mpos.x >= eyeMin.x && mpos.x <= eyeMax.x && mpos.y >= eyeMin.y && mpos.y <= eyeMax.y);

			ImVec2 pwMin(power_x, btn_y_abs);
			ImVec2 pwMax(power_x + btn_size, btn_y_abs + btn_size);
			bool pwrHovered = (mpos.x >= pwMin.x && mpos.x <= pwMax.x && mpos.y >= pwMin.y && mpos.y <= pwMax.y);

			bool eyeOpen = !Cheats::Settings::streamproof;
			ImU32 eyeBg = eyeHovered ? ImGui::GetColorU32(ImVec4(0.12f, 0.12f, 0.12f, 1.0f)) :
				eyeOpen ? ImGui::GetColorU32(ImVec4(0.08f, 0.08f, 0.08f, 1.0f)) :
				ImGui::GetColorU32(ImVec4(0.15f, 0.12f, 0.02f, 1.0f));
			ImU32 eyeBrdr = eyeOpen ? ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f)) :
				ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.4f));
			ImU32 eyeIco = eyeOpen ? ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)) :
				ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 1.0f));

			dl->AddRectFilled(eyeMin, eyeMax, eyeBg, 8.f);
			dl->AddRect(eyeMin, eyeMax, eyeBrdr, 8.f);

			const char* eyeIcon = eyeOpen ? ICON_EYE_FILL : ICON_EYE_CLOSE_FILL;
			ImVec2 eyeTS = font::icon_small->CalcTextSizeA(18.f, FLT_MAX, 0.f, eyeIcon);
			ImVec2 eyeTxt(eyeMin.x + (btn_size - eyeTS.x) * 0.5f, eyeMin.y + (btn_size - eyeTS.y) * 0.5f);
			dl->AddText(font::icon_small, 18.f, eyeTxt, eyeIco, eyeIcon);

			ImU32 pwrBg = pwrHovered ? ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.2f)) :
				ImGui::GetColorU32(ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
			ImU32 pwrBrdr = pwrHovered ? ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.5f)) :
				ImGui::GetColorU32(ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
			ImU32 pwrIco = pwrHovered ? ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 1.0f)) :
				ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

			dl->AddRectFilled(pwMin, pwMax, pwrBg, 8.f);
			dl->AddRect(pwMin, pwMax, pwrBrdr, 8.f);

			const char* pwrIconChar = ICON_POWER_FILL;
			ImVec2 pwTS = font::icon_small->CalcTextSizeA(18.f, FLT_MAX, 0.f, pwrIconChar);
			ImVec2 pwTxt(pwMin.x + (btn_size - pwTS.x) * 0.5f, pwMin.y + (btn_size - pwTS.y) * 0.5f);
			dl->AddText(font::icon_small, 18.f, pwTxt, pwrIco, pwrIconChar);

			ImGui::SetCursorPos(ImVec2(eye_x - pos.x, btn_y_abs - pos.y));
			if (ImGui::InvisibleButton("##eye_btn", ImVec2(btn_size, btn_size))) {
				Cheats::Settings::streamproof = !Cheats::Settings::streamproof;
			}

			ImGui::SetCursorPos(ImVec2(power_x - pos.x, btn_y_abs - pos.y));
			if (ImGui::InvisibleButton("##pwr_btn", ImVec2(btn_size, btn_size))) {
				exitLoop = true;
			}
		}

		float slide_x = content_x + tabSlideOffset;
		float slide_x2 = content_x + half_w + 12 + tabSlideOffset;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tabAlpha);

		if (p_tabs.IsTabActive(0)) {
			float subH = 34.f;
			float aimContentY = content_y + tab_offset + subH + 6.f;
			float aimContentH = content_h - subH - 6.f;
			const char* subNames[] = { "Aimbot", "Trigger", "Silent" };
			float subBtnW = content_w / 3.f;

			ImGui::SetCursorPos(ImVec2(content_x, content_y + tab_offset - 2));
			ImVec2 subScreenPos = ImGui::GetCursorScreenPos();
			ImDrawList* sdl = ImGui::GetWindowDrawList();
			ImVec2 subMin = subScreenPos;
			ImVec2 subMax(subMin.x + content_w, subMin.y + subH);
			sdl->AddRectFilled(subMin, subMax, ImGui::GetColorU32(ImVec4(0.06f, 0.06f, 0.06f, 1.f)), 10.f);
			sdl->AddRect(subMin, subMax, ImGui::GetColorU32(ImVec4(0.12f, 0.12f, 0.12f, 1.f)), 10.f);

			for (int si = 0; si < 3; si++) {
				ImVec2 btnMin(subMin.x + si * subBtnW, subMin.y);
				ImVec2 btnMax(btnMin.x + subBtnW, subMin.y + subH);
				bool isActive = (aimSubTab == si);
				if (isActive) {
					ImVec2 indMin(btnMin.x + 4.f, btnMin.y + 4.f);
					ImVec2 indMax(btnMin.x + subBtnW - 4.f, btnMin.y + subH - 4.f);
					sdl->AddRectFilled(indMin, indMax, ImGui::GetColorU32(ImVec4(0.15f, 0.12f, 0.02f, 1.f)), 8.f);
					sdl->AddRect(indMin, indMax, ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.4f)), 8.f);
				}
				ImVec2 textSize = ImGui::CalcTextSize(subNames[si]);
				ImVec2 textPos(btnMin.x + (subBtnW - textSize.x) * 0.5f, btnMin.y + (subH - textSize.y) * 0.5f);
				ImU32 textCol = isActive ? ImGui::GetColorU32(ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 1.f)) : ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 1.f));
				sdl->AddText(textPos, textCol, subNames[si]);

				ImGui::SetCursorPos(ImVec2(content_x + si * subBtnW, content_y + tab_offset - 2));
				if (ImGui::InvisibleButton(("##subtab_" + std::to_string(si)).c_str(), ImVec2(subBtnW, subH))) {
					aimSubTab = si;
				}
			}

			if (aimSubTab == 0) {
				ImGui::SetCursorPos(ImVec2(slide_x, aimContentY));
				ImVec2 c1p = ImGui::GetCursorScreenPos();
				DrawCardBg("Aimbot Settings", "Configure aimbot locking", c1p, ImVec2(half_w, aimContentH), ImGui::GetID("c1"));
				BeginCardContent(c1p, ImVec2(half_w, aimContentH), "##card_aim1");
					custom::Checkbox("Enable Aimbot", "Lock on to players.", &Cheats::Aim::aimBot);
					if (Cheats::Aim::aimBot) {
						custom::Checkbox("Draw Aimbot Fov", "Show FOV circle.", &Cheats::Aim::drawAimbotFov);
						custom::Combo("Aimbot Bone", &Cheats::Aim::aimbotSelectedType, Cheats::Aim::aimbotTypes, IM_ARRAYSIZE(Cheats::Aim::aimbotTypes), 3);
						custom::SliderInt("Aimbot Fov", &Cheats::Aim::aimBotFov, 1, 1000, "%d");
						custom::SliderInt("Aimbot Smoothing", &Cheats::Aim::aimBotSmooth, 1, 80, "%d");
						custom::Keybind("Keybind for aimbot", "Lock when pressed.", &Cheats::Aim::aimBotKey, 0);
					}
					else { Cheats::Aim::aimBotKey = 0; }
				EndCardContent();

				ImGui::SetCursorPos(ImVec2(slide_x2, aimContentY));
				ImVec2 c2p = ImGui::GetCursorScreenPos();
				DrawCardBg("Aim Filters", "Target selection rules", c2p, ImVec2(half_w, aimContentH), ImGui::GetID("c2"));
				BeginCardContent(c2p, ImVec2(half_w, aimContentH), "##card_aim2");
					custom::Checkbox("Only Visible", "Only visible targets.", &Cheats::Aim::onlyVisible);
					custom::Checkbox("Ignore Ped", "Skip peds.", &Cheats::Aim::ignorePed);
					custom::Checkbox("Ignore Death", "Skip dead.", &Cheats::Aim::ignoreDeath);
					custom::SliderInt("Max Distance", &Cheats::Aim::distance, 0, 500, "%dm");
					if (Cheats::Aim::drawAimbotFov) {
						custom::ColorEdit4("Aimbot Fov Color", "FOV color.", Cheats::Aim::drawAimbotFovColor, picker_flags);
					}
				EndCardContent();
			}
			else if (aimSubTab == 1) {
				ImGui::SetCursorPos(ImVec2(slide_x, aimContentY));
				ImVec2 c1p = ImGui::GetCursorScreenPos();
				DrawCardBg("Trigger Settings", "Auto-fire configuration", c1p, ImVec2(half_w, aimContentH), ImGui::GetID("c3"));
				BeginCardContent(c1p, ImVec2(half_w, aimContentH), "##card_trig1");
					custom::Checkbox("Enable Trigger Bot", "Auto fire.", &Cheats::Aim::triggerBot);
					if (Cheats::Aim::triggerBot) {
						custom::Checkbox("Draw Trigger Fov", "Show FOV.", &Cheats::Aim::drawTriggerFov);
						custom::Combo("Trigger Bone", &Cheats::Aim::triggerBotSelectedType, Cheats::Aim::triggerBotTypes, IM_ARRAYSIZE(Cheats::Aim::triggerBotTypes), 3);
						custom::Keybind("Keybind for Trigger", "Fire when pressed.", &Cheats::Aim::triggerKey, 0);
						custom::SliderInt("Trigger Fov", &Cheats::Aim::triggerFov, 1, 200, "%d");
						custom::SliderInt("Trigger Ms Delay", &Cheats::Aim::triggerDelay, 1, 10, "%d");
						custom::SliderInt("Crosshair Tolerance", &Cheats::Aim::crosshairTolerance, 30, 200, "%d");
					}
				EndCardContent();

				ImGui::SetCursorPos(ImVec2(slide_x2, aimContentY));
				ImVec2 c2p = ImGui::GetCursorScreenPos();
				DrawCardBg("Trigger Filters", "Auto-fire target rules", c2p, ImVec2(half_w, aimContentH), ImGui::GetID("c4"));
				BeginCardContent(c2p, ImVec2(half_w, aimContentH), "##card_trig2");
					custom::Checkbox("Only Visible", "Visible targets only.", &Cheats::Aim::onlyVisible);
					custom::Checkbox("Ignore Ped", "Skip peds.", &Cheats::Aim::ignorePed);
					custom::Checkbox("Ignore Death", "Skip dead.", &Cheats::Aim::ignoreDeath);
					custom::SliderInt("Max Distance", &Cheats::Aim::distance, 0, 500, "%dm");
					if (Cheats::Aim::drawTriggerFov)
						custom::ColorEdit4("Trigger Fov Color", "FOV color.", Cheats::Aim::drawTriggerFovColor, picker_flags);
				EndCardContent();
			}
			else if (aimSubTab == 2) {
				ImGui::SetCursorPos(ImVec2(slide_x, aimContentY));
				ImVec2 c1p = ImGui::GetCursorScreenPos();
				DrawCardBg("Silent Settings", "Invisible bullet redirection", c1p, ImVec2(half_w, aimContentH), ImGui::GetID("c5"));
				BeginCardContent(c1p, ImVec2(half_w, aimContentH), "##card_sil1");
					custom::Checkbox("Enable Silent", "Bullets redirect.", &Cheats::Aim::silent);
					custom::Checkbox("Enable Random Silent", "2/5 bullets hit.", &Cheats::Aim::Pslient);
					if (Cheats::Aim::silent) {
						custom::Checkbox("Draw Silent Fov", "Show FOV.", &Cheats::Aim::drawSilentFov);
						custom::Combo("Silent Bone", &Cheats::Aim::SlientSlectedType, Cheats::Aim::SlientTypes, IM_ARRAYSIZE(Cheats::Aim::SlientTypes), 4);
						custom::SliderInt("Silent Fov", &Cheats::Aim::silentFov, 0, 500, "%dm");
						custom::Keybind("Keybind for Silent", "Fire when pressed.", &Cheats::Aim::silentKey, 0);
					}
				EndCardContent();

				ImGui::SetCursorPos(ImVec2(slide_x2, aimContentY));
				ImVec2 c2p = ImGui::GetCursorScreenPos();
				DrawCardBg("Silent Filters", "Silent aim target rules", c2p, ImVec2(half_w, aimContentH), ImGui::GetID("c6"));
				BeginCardContent(c2p, ImVec2(half_w, aimContentH), "##card_sil2");
					custom::Checkbox("Only Visible", "Visible targets only.", &Cheats::Aim::onlyVisible);
					custom::Checkbox("Ignore Ped", "Skip peds.", &Cheats::Aim::ignorePed);
					custom::Checkbox("Ignore Death", "Skip dead.", &Cheats::Aim::ignoreDeath);
					custom::SliderInt("Max Distance", &Cheats::Aim::distance, 0, 500, "%dm");
					if (Cheats::Aim::drawSilentFov) {
						custom::ColorEdit4("Silent Fov Color", "FOV color.", Cheats::Aim::drawSilentFovColor, picker_flags);
					}
				EndCardContent();
			}
		}

		if (p_tabs.IsTabActive(1)) {
			ImGui::SetCursorPos(ImVec2(slide_x, content_y + tab_offset));
			ImVec2 c1p = ImGui::GetCursorScreenPos();
			DrawCardBg("Player ESP", "Visual overlays for players", c1p, ImVec2(half_w, content_h), ImGui::GetID("c7"));
			BeginCardContent(c1p, ImVec2(half_w, content_h), "##card_vis1");
				custom::Checkbox("Draw Player Name", "Show names.", &Cheats::Esp::nameEsp);
				custom::Checkbox("Draw Skeleton Esp", "Show bones.", &Cheats::Esp::skeletonEsp);
				custom::Checkbox("Draw Box Esp", "Show boxes.", &Cheats::Esp::boxEsp);
				custom::Checkbox("Draw Health Bar", "Show health.", &Cheats::Esp::healthBar);
				custom::Checkbox("Draw Armor Bar", "Show armor.", &Cheats::Esp::armorBar);
				custom::Checkbox("Draw Line Esp", "Show lines.", &Cheats::Esp::lineEsp);
				custom::Checkbox("Draw Weapon Esp", "Show weapons.", &Cheats::Esp::weaponEsp);
				custom::Checkbox("Draw Distance Esp", "Show distance.", &Cheats::Esp::distanceEsp);
				custom::Checkbox("Draw Direction Esp", "Show direction.", &Cheats::Esp::directionEsp);
			EndCardContent();

			ImGui::SetCursorPos(ImVec2(slide_x2, content_y + tab_offset));
			ImVec2 c2p = ImGui::GetCursorScreenPos();
			DrawCardBg("ESP Settings", "Customize ESP appearance", c2p, ImVec2(half_w, content_h), ImGui::GetID("c8"));
			BeginCardContent(c2p, ImVec2(half_w, content_h), "##card_vis2");
				custom::Checkbox("Only Visible", "Visible players only.", &Cheats::Esp::onlyVisible);
				custom::Checkbox("Ignore Ped", "Skip peds.", &Cheats::Esp::ignorePed);
				custom::Checkbox("Ignore Death", "Skip dead.", &Cheats::Esp::ignoreDeath);
				custom::SliderInt("Max Distance", &Cheats::Esp::distance, 0, 500, "%dm");
				if (Cheats::Esp::skeletonEsp) {
					custom::SliderInt("Skeleton Size", &Cheats::Esp::skeletonEspSize, 0, 20);
					custom::ColorEdit4("Skeleton Color", "Bone color.", Cheats::Esp::skeletonEspColor, picker_flags);
				}
				if (Cheats::Esp::boxEsp) {
					custom::ColorEdit4("Box Color", "Box color.", Cheats::Esp::boxEspColor, picker_flags);
					custom::Combo("Box Type", &Cheats::Esp::boxEspSelectedType, Cheats::Esp::boxEspTypes, IM_ARRAYSIZE(Cheats::Esp::boxEspTypes));
					custom::SliderFloat("Box Size", &Cheats::Esp::BoxSizeX, 0.75f, 1.202f, "Value: %.3f");
					custom::Checkbox("Enable Gradient Box", "Gradient effect.", &Cheats::Esp::gradientEnabled);
					if (Cheats::Esp::gradientEnabled) {
						custom::Checkbox("Custom Gradient", "Custom colors.", &Cheats::Esp::useCustomGradient);
						custom::SliderFloat("Box Gradient Intensity", &Cheats::Esp::gradientIntensity, 0.0f, 5.0f, "Value: %.2f");
						if (Cheats::Esp::useCustomGradient) {
							custom::ColorEdit4("Gradient Top Color", "Top color.", Cheats::Esp::boxGradientTopColor, picker_flags);
							custom::ColorEdit4("Gradient Bottom Color", "Bottom color.", Cheats::Esp::boxGradientBottomColor, picker_flags);
						}
					}
				}
				if (Cheats::Esp::lineEsp) {
					custom::ColorEdit4("Line Color", "Line color.", Cheats::Esp::lineEspColor, picker_flags);
					custom::Combo("Line Type", &Cheats::Esp::lineEspSelectedType, Cheats::Esp::lineEspTypes, IM_ARRAYSIZE(Cheats::Esp::lineEspTypes), 3);
				}
				if (Cheats::Esp::weaponEsp) { custom::ColorEdit4("Weapon Color", "Weapon color.", Cheats::Esp::weaponEspColor, picker_flags); }
				if (Cheats::Esp::distanceEsp) { custom::ColorEdit4("Distance Color", "Distance color.", Cheats::Esp::distanceEspColor, picker_flags); }
				if (Cheats::Esp::nameEsp) { custom::ColorEdit4("Name Color", "Name color.", Cheats::Esp::nameEspColor, picker_flags); }
				if (Cheats::Esp::directionEsp) { custom::ColorEdit4("Direction Color", "Direction color.", Cheats::Esp::directionEspColor, picker_flags); }
			EndCardContent();
		}

		if (p_tabs.IsTabActive(2)) {
			ImGui::SetCursorPos(ImVec2(slide_x, content_y + tab_offset));
			ImVec2 c1p = ImGui::GetCursorScreenPos();
			DrawCardBg("Movement", "Noclip, teleport and vehicle", c1p, ImVec2(half_w, content_h), ImGui::GetID("c9"));
			BeginCardContent(c1p, ImVec2(half_w, content_h), "##card_misc_mv");
				custom::Checkbox("No Clip Enabled", "Fly through walls.", &Cheats::Misc::NoClip);
				if (Cheats::Misc::NoClip) {
					custom::SliderInt("No Clip Speed", &Cheats::Misc::NoClipSpeed, 0, 70, "%dm/s");
					custom::Keybind("Keybind for No Clip", "Fly when pressed.", &Cheats::Misc::NoClipKey, 0);
				}
				if (custom::Button("Teleport To Waypoint", ImVec2(half_w - 24, 42))) {
					Cheats::Misc::TeleportWaypoint = true;
				}
				ImGui::Separator();
				custom::Checkbox("Enable Vehicle Esp", "Vehicle ESP.", &Cheats::Vehicle::vehicleEsp);
				if (Cheats::Vehicle::vehicleEsp) {
					custom::Checkbox("Draw Local Vehicle", "Local vehicle.", &Cheats::Vehicle::drawLocalVehicle);
					custom::Checkbox("Draw Enemy Vehicle", "Enemy vehicles.", &Cheats::Vehicle::drawEnemyVehicle);
					custom::Checkbox("Draw Vehicle Distance", "Vehicle distance.", &Cheats::Vehicle::vehicleEspShowDistance);
					custom::Checkbox("Draw Vehicle Marker", "Vehicle markers.", &Cheats::Vehicle::vehicleMarker);
					custom::Checkbox("Draw Vehicle Snapline", "Vehicle snaplines.", &Cheats::Vehicle::vehicleEspSnapline);
					custom::Checkbox("Draw Vehicle Health", "Vehicle health.", &Cheats::Vehicle::vehicleHealt);
					if (Cheats::Vehicle::vehicleEspShowDistance)
						custom::SliderInt("Draw Distance", &Cheats::Vehicle::distance, 30, 200, "%d");
				}
				ImGui::Separator();
				custom::Checkbox("Vehicle Fix", "Repair vehicle engine.", &Cheats::Misc::vehicleFix);
				custom::Checkbox("Unlock All Vehicles", "Unlock all vehicle doors.", &Cheats::Misc::vehicleUnlock);
			EndCardContent();

			ImGui::SetCursorPos(ImVec2(slide_x2, content_y + tab_offset));
			ImVec2 c2p = ImGui::GetCursorScreenPos();
			DrawCardBg("Player & Weapon", "Modifications and exploits", c2p, ImVec2(half_w, content_h), ImGui::GetID("c10"));
			BeginCardContent(c2p, ImVec2(half_w, content_h), "##card_misc_pw");
				custom::Checkbox("God Mode", "Smart restore (3s delay).", &Cheats::Misc::godMode);
				custom::Checkbox("Health Boost", "Full heal on press.", &Cheats::Misc::healthBoost);
				if (Cheats::Misc::healthBoost) { custom::Keybind("Health Boost Key", "Press to heal.", &Cheats::Misc::healthBoostKey, 0); }
				custom::Checkbox("Armor Boost", "Set armor on press.", &Cheats::Misc::armorBoost);
				if (Cheats::Misc::armorBoost) {
					custom::SliderInt("Armor Amount", &Cheats::Misc::armorBoostValue, 0, 100, "%d");
					custom::Keybind("Armor Boost Key", "Press for armor.", &Cheats::Misc::armorBoostKey, 0);
				}
				custom::Checkbox("FOV Changer", "Change FOV.", &Cheats::Misc::fovChanger);
				if (Cheats::Misc::fovChanger) { custom::SliderInt("Value", &Cheats::Misc::fovValue, 1, 100, "%d"); }
				ImGui::Separator();
				custom::Checkbox("Infinite Ammo", "Unlimited ammo.", &Cheats::Misc::infiniteAmmo);
				custom::Checkbox("No Recoil", "No recoil.", &Cheats::Misc::noRecoil);
				custom::Checkbox("No Spread", "No spread.", &Cheats::Misc::noSpread);
				custom::Checkbox("No Reload", "No reload.", &Cheats::Misc::noReload);
				custom::Checkbox("Damage Boost", "More damage.", &Cheats::Misc::damageBoost);
				if (Cheats::Misc::damageBoost) { custom::SliderInt("Damage Multiplier (10=1x, 100=10x, 200=20x)", &Cheats::Misc::damageValue, 1, 200, "%d"); }
				custom::Checkbox("Reload Ammo", "Refill ammo.", &Cheats::Misc::reloadAmmo);
				if (Cheats::Misc::reloadAmmo) {
					custom::SliderInt("Ammo Refill Amount", &Cheats::Misc::reloadValue, 1, 30, "%d");
					custom::Keybind("Reload Ammo Key", "Refill when pressed.", &Cheats::Misc::reloadAmmoKey, 0);
				}
			EndCardContent();
		}

		if (p_tabs.IsTabActive(3)) {
			ImGui::SetCursorPos(ImVec2(slide_x, content_y + tab_offset));
			ImVec2 c1p = ImGui::GetCursorScreenPos();
			DrawCardBg("Profile", "License and build info", c1p, ImVec2(half_w, content_h), ImGui::GetID("c_prof"));
			BeginCardContent(c1p, ImVec2(half_w, content_h), "##card_profile");
				{
					ImDrawList* dl = ImGui::GetWindowDrawList();
					float cw = ImGui::GetContentRegionAvail().x;
					float boxPadX = 12.f;
					float boxH = 52.f;
					float boxSpacing = 8.f;
					float boxW = (cw - boxPadX) / 2.f;

					auto drawInfoBox = [&](const char* label, const char* value, ImVec2 boxMin, ImVec2 boxMax, bool accent) {
						ImU32 boxBg = ImGui::GetColorU32(ImVec4(0.07f, 0.07f, 0.07f, 1.0f));
						ImU32 boxBorder = ImGui::GetColorU32(ImVec4(0.13f, 0.13f, 0.14f, 1.0f));
						dl->AddRectFilled(boxMin, boxMax, boxBg, 10.f);
						dl->AddRect(boxMin, boxMax, boxBorder, 10.f);
						ImGui::PushFont(font::small_font);
						dl->AddText(ImVec2(boxMin.x + 10.f, boxMin.y + 8.f), utils::ToU32(Theme::TextDim), label);
						ImU32 valCol = accent ? utils::ToU32(Theme::Accent) : utils::ToU32(Theme::TextPrimary);
						dl->AddText(ImVec2(boxMin.x + 10.f, boxMin.y + 26.f), valCol, value);
						ImGui::PopFont();
					};

					float row0Y = ImGui::GetCursorScreenPos().y;
					float c1x = ImGui::GetCursorScreenPos().x;

					drawInfoBox("BUILD", "alp/fy0w", ImVec2(c1x, row0Y), ImVec2(c1x + boxW, row0Y + boxH), false);
					drawInfoBox("KEY NAME", Cheats::Auth::keyName.c_str(), ImVec2(c1x + boxW + boxPadX, row0Y), ImVec2(c1x + cw, row0Y + boxH), true);
					float row1Y = row0Y + boxH + boxSpacing;
					drawInfoBox("KEY DATE", Cheats::Auth::keyDate.c_str(), ImVec2(c1x, row1Y), ImVec2(c1x + boxW, row1Y + boxH), false);
					drawInfoBox("USER", Cheats::Auth::username.c_str(), ImVec2(c1x + boxW + boxPadX, row1Y), ImVec2(c1x + cw, row1Y + boxH), false);
					float row2Y = row1Y + boxH + boxSpacing;
					drawInfoBox("MENU KEY", "F9", ImVec2(c1x, row2Y), ImVec2(c1x + boxW, row2Y + boxH), true);
					drawInfoBox("STATUS", Cheats::Auth::authenticated ? "Authenticated" : "Local", ImVec2(c1x + boxW + boxPadX, row2Y), ImVec2(c1x + cw, row2Y + boxH), false);
					ImGui::SetCursorPosY(row2Y + boxH + 12.f);
				}
			EndCardContent();

			ImGui::SetCursorPos(ImVec2(slide_x2, content_y + tab_offset));
			ImVec2 c2p = ImGui::GetCursorScreenPos();
			DrawCardBg("Overlay", "Visual configuration", c2p, ImVec2(half_w, content_h), ImGui::GetID("c_over"));
			BeginCardContent(c2p, ImVec2(half_w, content_h), "##card_overlay");
				custom::Checkbox("Watermark", "Show watermark.", &Cheats::Settings::watermark);
				custom::Checkbox("Streamproof", "Hide from capture.", &Cheats::Settings::streamproof);
				custom::Checkbox("VSync", "Sync FPS.", &Cheats::Settings::vsync);
				
				ImGui::Separator();
				
				
				ImGui::PushFont(font::s_inter_semibold);
				ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 0.9f), "Menu Color");
				ImGui::PopFont();
				
				const char* colorNames[] = { "Yellow", "Blue", "Purple", "Red", "Green", "Pink" };
				ImVec4 colorValues[] = {
					ImVec4(1.0f, 0.75f, 0.14f, 1.0f), 
					ImVec4(0.2f, 0.6f, 1.0f, 1.0f),   
					ImVec4(0.7f, 0.3f, 0.9f, 1.0f),    
					ImVec4(0.9f, 0.2f, 0.2f, 1.0f),    
					ImVec4(0.3f, 0.8f, 0.3f, 1.0f),    
					ImVec4(1.0f, 0.4f, 0.7f, 1.0f),    
				};
				
				
				float btnSize = 28.f;
				float btnSpacing = 8.f;
				float totalWidth = 6 * btnSize + 5 * btnSpacing;
				float startX = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + startX);
				
				for (int i = 0; i < 6; i++) {
					ImGui::PushID(i);
					
					ImVec2 btnMin = ImGui::GetCursorScreenPos();
					ImVec2 btnMax(btnMin.x + btnSize, btnMin.y + btnSize);
					
					bool isSelected = (Cheats::Settings::selectedColor == i);
					ImU32 bgColor = isSelected ? ImGui::GetColorU32(colorValues[i]) : ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
					ImU32 borderColor = ImGui::GetColorU32(colorValues[i]);
					
					ImGui::GetWindowDrawList()->AddRectFilled(btnMin, btnMax, bgColor, 6.f);
					ImGui::GetWindowDrawList()->AddRect(btnMin, btnMax, borderColor, 6.f, 0, 2.f);
					
					if (isSelected) {
						ImVec2 checkCenter(btnMin.x + btnSize * 0.5f, btnMin.y + btnSize * 0.5f);
						ImGui::GetWindowDrawList()->AddText(checkCenter, ImGui::GetColorU32(ImVec4(1.f, 1.f, 1.f, 1.f)), ICON_CHECK_LINE);
					}
					
					if (ImGui::InvisibleButton(("##color_" + std::to_string(i)).c_str(), ImVec2(btnSize, btnSize))) {
						Cheats::Settings::selectedColor = i;
						
						Theme::Accent = colorValues[i];
						Theme::AccentDark = ImVec4(colorValues[i].x * 0.8f, colorValues[i].y * 0.8f, colorValues[i].z * 0.8f, 1.0f);
						Theme::AccentGlow = ImVec4(colorValues[i].x, colorValues[i].y, colorValues[i].z, 0.15f);
						Theme::ToggleOn = ImVec4(colorValues[i].x, colorValues[i].y, colorValues[i].z, 0.9f);
						Theme::SliderFill = colorValues[i];
						Theme::ButtonHover = colorValues[i];
						Theme::SidebarBorder = ImVec4(colorValues[i].x, colorValues[i].y, colorValues[i].z, 0.7f);
						Theme::SidebarActive = ImVec4(colorValues[i].x * 0.15f, colorValues[i].y * 0.15f, colorValues[i].z * 0.15f, 0.7f);
						c::accent = colorValues[i];
						c::anim::active = ImColor(colorValues[i]);
						c::sidebar::text_active = ImColor(colorValues[i]);
						c::sidebar::item_active_bg = ImColor(colorValues[i].x * 0.15f, colorValues[i].y * 0.15f, colorValues[i].z * 0.15f, 0.7f);
						c::sidebar::active_border = ImColor(colorValues[i].x, colorValues[i].y, colorValues[i].z, 0.7f);
						c::toggle::bg_on = ImColor(colorValues[i].x, colorValues[i].y, colorValues[i].z, 0.9f);
						c::checkbox::mark = colorValues[i];
					}
					
					ImGui::SameLine(0, btnSpacing);
					ImGui::PopID();
				}
				ImGui::NewLine();

				ImGui::SetCursorPosY(ImGui::GetContentRegionMax().y - 108);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.65f, 0.0f, 0.8f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.5f, 0.0f, 1.0f));
				if (custom::Button(ICON_DELETE_2_LINE "  Unload", ImVec2(ImGui::GetContentRegionAvail().x, 42))) {
					exitLoop = true;
				}
				ImGui::PopStyleColor(3);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.15f, 0.15f, 0.85f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
				if (custom::Button(ICON_DELETE_2_LINE "  Safe Destruct", ImVec2(ImGui::GetContentRegionAvail().x, 42))) {
					destructActive = true;
					isMenuVisible = false;
					destructStartTime = (float)ImGui::GetTime();
					destructCleanupDone = false;
				}
				ImGui::PopStyleColor(3);
			EndCardContent();
		}

		ImGui::PopStyleVar();

		{
			struct SnowFlake { float x; float y; float speed; float size; float drift; float phase; };
			static SnowFlake flakes[40];
			static bool initSnow = false;
			if (!initSnow) {
				srand(12345);
				for (int i = 0; i < 40; i++) {
					flakes[i].x = (float)(rand() % 1000) / 1000.0f;
					flakes[i].y = (float)(rand() % 1000) / 1000.0f;
				flakes[i].speed = 0.002f + (float)(rand() % 100) / 100000.0f;
				flakes[i].size = 0.5f + (float)(rand() % 100) / 60.0f;
					flakes[i].drift = 0.003f + (float)(rand() % 100) / 15000.0f;
					flakes[i].phase = (float)(rand() % 1000) / 100.0f;
				}
				initSnow = true;
			}
			float time = (float)ImGui::GetTime();
			float content_x_abs = pos.x + sidebar_w + 16.f;
			float content_w_abs = c::bg::size.x - sidebar_w - 32.f;
			for (int i = 0; i < 40; i++) {
				SnowFlake& sf = flakes[i];
				float sy = fmodf(sf.y + sf.speed, 1.1f);
				sf.y = sy;
				float sx = sf.x + sinf(time * 0.1f + sf.phase) * sf.drift * 5.0f;
				sx = fmodf(sx + 1.0f, 1.0f);
				float px = content_x_abs + sx * content_w_abs;
				float py = pos.y + sy * c::bg::size.y;
				float flicker = 0.7f + sinf(time * 0.5f + sf.phase) * 0.3f;
				dl->AddCircleFilled(ImVec2(px, py), sf.size * 0.5f, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, flicker * 0.6f)), 6);
			}
		}
	}
	End();
}
