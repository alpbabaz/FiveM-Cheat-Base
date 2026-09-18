#pragma once

#include <iostream>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_settings.h>

#include <Windows.h>
#include <vector>
#include <string>
enum notif_state
{
	enabling,
	waiting,
	disabling
};

static std::vector<std::string> general_text;
static std::vector<std::string> icons;
static std::vector<ImColor> color;
static std::vector<ImVec2> position;
static std::vector<notif_state> state;

class CNotifications
{
private:

public:

	void AddMessage(const char* name, const char* icon, ImColor icon_color)
	{
		general_text.push_back(name);
		color.push_back(icon_color);
		icons.push_back(icon);
		state.push_back(notif_state::enabling);
		position.push_back(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y));
	}

	void Render()
	{
		static DWORD dwTickStart = GetTickCount();

		for (int i = 0; i < general_text.size(); /* индекс не увеличиваем здесь */)
		{
			
			position[i].x = ImLerp(position[i].x, state[i] == notif_state::enabling || state[i] == notif_state::waiting ? ImGui::GetIO().DisplaySize.x - ImGui::CalcTextSize(general_text[i].c_str()).x - 105 : ImGui::GetIO().DisplaySize.x + 200, ImGui::GetAnimSpeed());

			
			position[i].y = ImGui::GetIO().DisplaySize.y - (i + 1) * 70;

			if (GetTickCount() - dwTickStart > 1500)
			{
				if (state[i] == notif_state::enabling) {
					state[i] = notif_state::waiting;
				}
				else if (state[i] == notif_state::waiting) {
					state[i] = notif_state::disabling;
				}
				dwTickStart = GetTickCount();
			}

			if (state[i] == notif_state::disabling && position[i].x > ImGui::GetIO().DisplaySize.x + 190)
			{
				state.erase(state.begin() + i);
				position.erase(position.begin() + i);
				color.erase(color.begin() + i);
				general_text.erase(general_text.begin() + i);
				continue;
			}

			
			ImGui::GetForegroundDrawList()->AddRectFilled(position[i] + ImVec2(20, 0), position[i] + ImVec2(60, 30) + ImGui::CalcTextSize(general_text[i].c_str()) + ImVec2(ImGui::CalcTextSize(icons[i].c_str()).x, 0), ImGui::GetColorU32(c::window_bg_color), 4.f);
			ImGui::GetForegroundDrawList()->AddRect(position[i] + ImVec2(20, 0), position[i] + ImVec2(60, 30) + ImGui::CalcTextSize(general_text[i].c_str()) + ImVec2(ImGui::CalcTextSize(icons[i].c_str()).x, 0), ImGui::GetColorU32(c::child::stroke), 4.f);

			ImGui::GetForegroundDrawList()->AddText(position[i] + ImVec2(40.f + ImGui::CalcTextSize(icons[i].c_str()).x, 15.5f), c::label::active, general_text[i].c_str());
			ImGui::GetForegroundDrawList()->AddText(position[i] + ImVec2(30, 15.5f), color[i], icons[i].c_str());

			++i;
		}
	}
};