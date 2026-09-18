#include "../Overlay/OverlayDraw.hpp"
#include "GameSDK.hpp"
#include "Ped.hpp"
#include <unordered_map>
#include <mutex>
 
Ped localPlayer;
uintptr_t gameWorld;
uintptr_t viewPort;
std::vector<Ped> pedList;
bool TPVehicle = false;
uintptr_t TPModelInfo = NULL;
uintptr_t TPNavigation = NULL;
Vector3 TPPosition = Vector3(0, 0, 0);

struct PedBarFix {
	int id;
	float health;
	float armor;
};

std::vector<PedBarFix> pedBarFix;

void DrawAlways() {
	ImDrawList* drawList = ImGui::GetForegroundDrawList();
	if (Cheats::Aim::crossHair) {
		const ImVec2 center = { Game.lpRect.right / 2.f, Game.lpRect.bottom / 2.f };
		const ImColor crossHairColor = ConvertToImColor(Cheats::Aim::crossHairColor);
		const float size = Cheats::Aim::crossHairSize;

		switch (Cheats::Aim::crossHairSelectedType) {
		case 0: 
		{
			const float gapSize = 4.0f;
			const float dotSize = 2.0f;
			drawList->AddLine({ center.x - size - gapSize, center.y }, { center.x - gapSize, center.y }, crossHairColor, 1.5f);
			drawList->AddLine({ center.x + gapSize, center.y }, { center.x + size + gapSize, center.y }, crossHairColor, 1.5f);
			drawList->AddLine({ center.x, center.y - size - gapSize }, { center.x, center.y - gapSize }, crossHairColor, 1.5f);
			drawList->AddLine({ center.x, center.y + gapSize }, { center.x, center.y + size + gapSize }, crossHairColor, 1.5f);
			drawList->AddCircleFilled(center, dotSize, ImColor(255, 255, 255));  
		}
		break;

		case 1: 
		{
			drawList->AddLine({ center.x, center.y - size }, { center.x, center.y + size }, crossHairColor, 1.5f);
			drawList->AddLine({ center.x - size, center.y }, { center.x + size, center.y }, crossHairColor, 1.5f);
		}
		break;

		case 2: 
		{
			drawList->AddLine({ center.x - size, center.y - size }, { center.x + size, center.y + size }, crossHairColor, 1.5f);
			drawList->AddLine({ center.x + size, center.y - size }, { center.x - size, center.y + size }, crossHairColor, 1.5f);
		}
		break;

		case 3: 
		{
			const float radius = size / 2.f;
			drawList->AddCircle(center, radius, crossHairColor, 50, 1.5f);
			drawList->AddCircleFilled(center, radius / 2.f, crossHairColor);
		}
		break;

		case 4: 
		{
			const float radius = size;
			const int spokeCount = 8;  
			for (int i = 0; i < spokeCount; ++i) {
				const float angle = (360.f / spokeCount) * i * (3.14159265359f / 180.0f);
				ImVec2 p1 = { center.x + radius * cos(angle), center.y + radius * sin(angle) };
				ImVec2 p2 = { center.x - radius * cos(angle), center.y - radius * sin(angle) };
				drawList->AddLine(p1, p2, crossHairColor, 1.5f);
			}
		}
		break;

		case 5: 
		{
			const float diamondSize = size;
			const float halfSize = diamondSize * 0.5f;
			ImVec2 p1 = { center.x - halfSize, center.y };
			ImVec2 p2 = { center.x, center.y - halfSize };
			ImVec2 p3 = { center.x + halfSize, center.y };
			ImVec2 p4 = { center.x, center.y + halfSize };
			drawList->AddLine(p1, p2, crossHairColor, 1.5f);
			drawList->AddLine(p2, p3, crossHairColor, 1.5f);
			drawList->AddLine(p3, p4, crossHairColor, 1.5f);
			drawList->AddLine(p4, p1, crossHairColor, 1.5f);
		}
		break;

		case 6:  
		{
			const float outerRadius = size;
			const float innerRadius = outerRadius * 0.6f;
			drawList->AddCircle(center, outerRadius, crossHairColor, 50, 2.0f);
			drawList->AddCircle(center, innerRadius, crossHairColor, 50, 1.5f);
			drawList->AddCircleFilled(center, innerRadius * 0.5f, crossHairColor);
		}
		break;

		case 7:  
		{
			const float sawRadius = size;
			const int sawTeeth = 12;
			const float innerRadius = sawRadius * 0.7f;
			for (int i = 0; i < sawTeeth; ++i) {
				const float angle1 = (360 / sawTeeth * i) * (3.14159265359f / 180.0f);
				const float angle2 = (360 / sawTeeth * (i + 1)) * (3.14159265359f / 180.0f);
				ImVec2 p1 = { center.x + sawRadius * cos(angle1), center.y + sawRadius * sin(angle1) };
				ImVec2 p2 = { center.x + innerRadius * cos(angle2), center.y + innerRadius * sin(angle2) };
				drawList->AddLine(p1, p2, crossHairColor, 1.5f);
			}
		}
		break;

		case 8:  
		{
			const float webRadius = size;
			const int webLines = 6;
			const int webRings = 3;
			for (int i = 0; i < webLines; ++i) {
				const float angle = (360 / webLines * i) * (3.14159265359f / 180.0f);
				ImVec2 end = { center.x + webRadius * cos(angle), center.y + webRadius * sin(angle) };
				drawList->AddLine(center, end, crossHairColor, 1.5f);
			}
			for (int r = 1; r <= webRings; ++r) {
				const float ringRadius = (webRadius / webRings) * r;
				drawList->AddCircle(center, ringRadius, crossHairColor, 50, 1.5f);
			}
		}
		break;

		case 9:  
		{
			const float outerRadius = size;
			const float innerRadius = outerRadius * 0.8f;
			drawList->AddCircle(center, outerRadius, crossHairColor, 100, 3.0f);
			drawList->AddCircle(center, innerRadius, crossHairColor, 100, 1.5f);
			drawList->AddCircleFilled(center, innerRadius * 0.5f, ImColor(255, 255, 255, 150));
		}
		break;

		default:
			break;
		}
	}

	if (Cheats::Aim::drawAimbotFov) {
		drawList->AddCircle(ImVec2(Game.lpRect.right / 2.f, Game.lpRect.bottom / 2.f), Cheats::Aim::aimBotFov, ConvertToImColor(Cheats::Aim::drawAimbotFovColor), 100, 1.0f);
	}
	if (Cheats::Aim::drawSilentFov) {
		drawList->AddCircle(ImVec2(Game.lpRect.right / 2.f, Game.lpRect.bottom / 2.f), Cheats::Aim::silentFov, ConvertToImColor(Cheats::Aim::drawSilentFovColor), 100, 1.0f);
	}
	if (Cheats::Aim::drawTriggerFov) {
		drawList->AddCircle(ImVec2(Game.lpRect.right / 2.f, Game.lpRect.bottom / 2.f), Cheats::Aim::triggerFov, ConvertToImColor(Cheats::Aim::drawTriggerFovColor), 100, 1.0f);
	}
}
 
namespace GetPlayerNames {
	uintptr_t citizenbase_address = 0;
	
	
	inline std::unordered_map<int, std::string> PlayerIdToName_Table;
	inline std::mutex NameMapMutex;
	
	
	inline uintptr_t hashTableAddr = 0;
	inline uintptr_t sentinelAddr = 0;
	inline uintptr_t hashMaskAddr = 0;

	inline void UpdatePlayerNames() {
		while (exitLoop == false) {
			try {
				if (citizenbase_address == 0) {
					citizenbase_address = GetBaseAddress("citizen-playernames-five.dll");
					if (citizenbase_address == 0) {
						Sleep(5000);
						continue;
					}
				}

				if (!hashTableAddr) {
					static const uintptr_t offs[] = { 0x30DA8, 0x34DA8, 0x30D38, 0x34D28, 0x30D48 };
					for (auto off : offs) {
						uintptr_t v = ReadMemory<uintptr_t>(citizenbase_address + off);
						if (v > 0x10000 && v < 0x7FFFFFFFFFFF) {
							uint64_t tryMask = ReadMemory<uint64_t>(citizenbase_address + off + 0x18);
							if (tryMask > 0 && tryMask < 0x10000) {
								hashTableAddr = citizenbase_address + off;
								sentinelAddr = citizenbase_address + off - 0x10;
								hashMaskAddr = citizenbase_address + off + 0x18;
								break;
							}
						}
					}
					if (!hashTableAddr) { Sleep(3000); continue; }
				}

				uintptr_t tablePtr = ReadMemory<uintptr_t>(hashTableAddr);
				uintptr_t sentinel = ReadMemory<uintptr_t>(sentinelAddr);
				uint64_t mask = ReadMemory<uint64_t>(hashMaskAddr);

				if (!tablePtr || tablePtr < 0x1000 || mask == 0) {
					Sleep(1000); continue;
				}

				std::unordered_map<int, std::string> newCache;

				for (uint64_t bucket = 0; bucket <= mask && bucket < 4096; bucket++) {
					uintptr_t node = ReadMemory<uintptr_t>(tablePtr + bucket * 16 + 8);
					if (!node || node < 0x1000 || node == sentinel) continue;

					int safety = 0;
					while (node && node > 0x1000 && node != sentinel && safety++ < 64) {
						uint32_t rawId = ReadMemory<uint32_t>(node + 0x10);
						uint16_t netId = (uint16_t)(rawId ^ 0xFEED);

						if (netId > 0 && netId < 65535) {
							std::string name = ReadString(node + 0x18);
							if (!name.empty() && name != "** Invalid **") {
								newCache[netId] = name;
								newCache[rawId] = name;
							}
						}

						uintptr_t next = ReadMemory<uintptr_t>(node + 0x8);
						if (next == node || next < 0x1000) break;
						node = next;
					}
				}

				if (!newCache.empty()) {
					std::lock_guard<std::mutex> lock(NameMapMutex);
					PlayerIdToName_Table = std::move(newCache);
				}
			}
			catch (...) {
			}
			Sleep(2000);
		}
	}

	inline std::string GetPedName(Ped& ped) {
		if (!ped.update()) return "Unknown";
		
		int playerId = ped.getID();
		uint16_t netId = (uint16_t)(playerId ^ 0xFEED);
		
		std::lock_guard<std::mutex> lock(NameMapMutex);
		
		
		auto it = PlayerIdToName_Table.find(netId);
		if (it != PlayerIdToName_Table.end()) return it->second;
		
		
		it = PlayerIdToName_Table.find(playerId);
		if (it != PlayerIdToName_Table.end()) return it->second;
		
		return "Unknown";
	}
}

  
void DrawEsp() { 
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	Matrix viewMatrix = ReadMemory<Matrix>(viewPort + 0x24C);
	if (!localPlayer.update()) {
		return;
	}

	for (auto& ped : pedList) {
		if (!ped.update()) {
			continue;
		}

 		float pDistance = getDistance(ped.position, localPlayer.position);
		if (pDistance >= Cheats::Esp::distance) {
			continue;
		}
		 
		if (Cheats::Esp::ignorePed && !ped.isPlayer()) {
			continue;
		}

		if (Cheats::Esp::ignoreDeath && ped.isDead()) {
			continue;
		}

		if(Cheats::Esp::onlyVisible && !ped.isVisible())
			continue;

		Vector2 pBase{}, pHead{}, pNeck{}, pLeftFoot{}, pRightFoot{};
		if (!worldToScreen(viewMatrix, ped.position, pBase) ||
			!worldToScreen(viewMatrix, ped.boneList[Head], pHead) ||
			!worldToScreen(viewMatrix, ped.boneList[Neck], pNeck) ||
			!worldToScreen(viewMatrix, ped.boneList[LeftFoot], pLeftFoot) ||
			!worldToScreen(viewMatrix, ped.boneList[RightFoot], pRightFoot)) {
			continue;
		}

		float HeadToNeck = pNeck.y - pHead.y;
		float pTop = pHead.y - (HeadToNeck * 2.5f);
		float pBottom = pLeftFoot.y > pRightFoot.y ? pLeftFoot.y : pRightFoot.y;
		float pHeight = pBottom - pTop;
		float pWidth = pHeight / 3.5f;
		float bScale = pWidth / 1.5f;

		if (Cheats::Esp::skeletonEsp) {
			ImColor color = ConvertToImColor(Cheats::Esp::skeletonEspColor);
			if (!ped.isVisible()) {
				color = ImColor(255,255,255);
			}

			Vector2 screenHeadPos;
			for (int j = 0; j < 5; j++) {
				Vector3 skeletonList[][2] = {
					{ ped.boneList[Neck], ped.boneList[Hip] },
					{ ped.boneList[Neck], ped.boneList[LeftHand] },
					{ ped.boneList[Neck], ped.boneList[RightHand] },
					{ ped.boneList[Hip], ped.boneList[LeftFoot] },
					{ ped.boneList[Hip], ped.boneList[RightFoot] }
				};

				Vector2 ScreenB1, ScreenB2;
				if (vec3Empty(skeletonList[j][0]) || vec3Empty(skeletonList[j][1])) {
					break;
				}
				else if (!worldToScreen(viewMatrix, skeletonList[j][0], ScreenB1) || !worldToScreen(viewMatrix, skeletonList[j][1], ScreenB2)) {
					break;
				}

				DrawLine(drawList, ImVec2(ScreenB1.x, ScreenB1.y), ImVec2(ScreenB2.x, ScreenB2.y), ImColor(0, 0, 0, 255), Cheats::Esp::skeletonEspSize + 2);
				DrawLine(drawList, ImVec2(ScreenB1.x, ScreenB1.y), ImVec2(ScreenB2.x, ScreenB2.y), color, Cheats::Esp::skeletonEspSize);
			}
		}
		float reducedWidth = pWidth * Cheats::Esp::BoxSizeX;   
		if (Cheats::Esp::boxEsp) {
			ImColor color = ConvertToImColor(Cheats::Esp::boxEspColor);
			if (!ped.isVisible()) {
				color = DarkenColor(color, 0.3f);
			}

			ImVec4 mainColor = color.Value;

			auto DrawBoxWithGradient = [&](bool isCornerBox) {
				if (Cheats::Esp::gradientEnabled) {
					ImVec4 topColor, bottomColor;

					if (Cheats::Esp::useCustomGradient) {
						topColor = ImVec4(
							Cheats::Esp::boxGradientTopColor[0],
							Cheats::Esp::boxGradientTopColor[1],
							Cheats::Esp::boxGradientTopColor[2],
							Cheats::Esp::boxGradientTopColor[3] * Cheats::Esp::gradientIntensity
						);
						bottomColor = ImVec4(
							Cheats::Esp::boxGradientBottomColor[0],
							Cheats::Esp::boxGradientBottomColor[1],
							Cheats::Esp::boxGradientBottomColor[2],
							Cheats::Esp::boxGradientBottomColor[3] * Cheats::Esp::gradientIntensity
						);
					}
					else {

						float intensity = Cheats::Esp::gradientIntensity;
						topColor = ImVec4(
							mainColor.x * 0.6f,
							mainColor.y * 0.6f,
							mainColor.z * 0.6f,
							0.25f * intensity
						);
						bottomColor = ImVec4(
							mainColor.x * 0.1f,
							mainColor.y * 0.1f,
							mainColor.z * 0.1f,
							0.35f * intensity
						);
					}

					drawList->AddRectFilledMultiColor(
						ImVec2(pBase.x - reducedWidth + 1, pTop + 1),
						ImVec2(pBase.x + reducedWidth - 1, pBottom - 1),
						ImGui::ColorConvertFloat4ToU32(topColor),
						ImGui::ColorConvertFloat4ToU32(topColor),
						ImGui::ColorConvertFloat4ToU32(bottomColor),
						ImGui::ColorConvertFloat4ToU32(bottomColor)
					);
				}

				if (!isCornerBox) {
					DrawLineOutline(ImVec2(pBase.x - reducedWidth, pTop), ImVec2(pBase.x + reducedWidth, pTop), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x - reducedWidth, pTop), ImVec2(pBase.x - reducedWidth, pBottom), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x + reducedWidth, pTop), ImVec2(pBase.x + reducedWidth, pBottom), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x - reducedWidth, pBottom), ImVec2(pBase.x + reducedWidth, pBottom), color, 1.f);
				}
				else {
					DrawLineOutline(ImVec2((pBase.x - reducedWidth), pTop), ImVec2((pBase.x - reducedWidth) + bScale, pTop), color, 1.f);
					DrawLineOutline(ImVec2((pBase.x + reducedWidth), pTop), ImVec2((pBase.x + reducedWidth) - bScale, pTop), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x - reducedWidth, pTop), ImVec2(pBase.x - reducedWidth, pTop + bScale), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x - reducedWidth, pBottom), ImVec2(pBase.x - reducedWidth, pBottom - bScale), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x + reducedWidth, pTop), ImVec2(pBase.x + reducedWidth, pTop + bScale), color, 1.f);
					DrawLineOutline(ImVec2(pBase.x + reducedWidth, pBottom), ImVec2(pBase.x + reducedWidth, pBottom - bScale), color, 1.f);
					DrawLineOutline(ImVec2((pBase.x - reducedWidth), pBottom), ImVec2((pBase.x - reducedWidth) + bScale, pBottom), color, 1.f);
					DrawLineOutline(ImVec2((pBase.x + reducedWidth), pBottom), ImVec2((pBase.x + reducedWidth) - bScale, pBottom), color, 1.f);
				}
				};

			if (Cheats::Esp::boxEspSelectedType == 0) {
				DrawBoxWithGradient(false);
			}
			else if (Cheats::Esp::boxEspSelectedType == 1) {
				DrawBoxWithGradient(true);
			}
		}

		if (Cheats::Esp::lineEsp) {
			ImColor color = ConvertToImColor(Cheats::Esp::lineEspColor);
			if (!ped.isVisible()) {
				color = DarkenColor(color, 0.3f);
			}

			ImVec2 startPos, endPos;
			ImGuiIO& io = ImGui::GetIO();
			if (Cheats::Esp::lineEspSelectedType == 0) {
				startPos.x = pBase.x + reducedWidth;
				startPos.y = pTop;
				endPos.x = Game.lpRect.right / 2.f;
				endPos.y = 0;
			}
			if (Cheats::Esp::lineEspSelectedType == 1) {
				startPos.x = pBase.x + reducedWidth;
				startPos.y = pTop;
				endPos.x = io.DisplaySize.x / 2.0f;
				endPos.y = io.DisplaySize.y / 2.0f;
			}
			if (Cheats::Esp::lineEspSelectedType == 2) {
				startPos.x = pBase.x;
				startPos.y = pBottom;
				endPos.x = io.DisplaySize.x / 2.0f;
				endPos.y = io.DisplaySize.y;
			}
			DrawLineOutline(startPos, endPos, color, 1.f);
		}

		ImVec2 pos1 = ImVec2(pBase.x, pBase.y + pHeight / 2 + 8);
		ImVec2 pos2 = ImVec2(pBase.x, pBase.y + pHeight / 2 + 22);
		ImVec2 pos3 = ImVec2(pBase.x, pBase.y + pHeight / 2 + 35);
		if (Cheats::Esp::weaponEsp) {
			ImColor color = ConvertToImColor(Cheats::Esp::weaponEspColor);
			if (!ped.isVisible()) {
				color = DarkenColor(color, 0.3f);
			}
			DrawShadowedTexts(drawList, "." + getWeaponName(ped.getWeapon()), pos2 + ImVec2(1, 1), 15.0f, ImColor(0, 0, 0, 150), true);
			DrawShadowedTexts(drawList, "." + getWeaponName(ped.getWeapon()), pos2, 15.0f, color, true);
		}
		if (Cheats::Esp::nameEsp) { 
			ImColor color = ConvertToImColor(Cheats::Esp::nameEspColor);
			if (!ped.isVisible()) {
				color = DarkenColor(color, 0.3f);
			}
			std::string pedName = GetPlayerNames::GetPedName(ped);
			DrawShadowedTexts(drawList, pedName.c_str(), pos1 + ImVec2(1, 1), 15.0f, ImGui::GetColorU32(ImVec4(0, 0, 0, 150)), true);
			DrawShadowedTexts(drawList, pedName.c_str(), pos1, 15.0f, color, true);

		}
		if (Cheats::Esp::distanceEsp) {
			ImColor color = ConvertToImColor(Cheats::Esp::distanceEspColor);
			if (!ped.isVisible()) {
				color = DarkenColor(color, 0.3f);
			}
			if (!Cheats::Esp::weaponEsp || getWeaponName(ped.getWeapon()).empty()) {
				pos3 = pos2;
			}
			std::string dataText = "[" + std::to_string((int)pDistance) + "m]";
			DrawShadowedTexts(ImGui::GetBackgroundDrawList(), dataText, pos3 + ImVec2(1, 1), 15.0f, ImGui::GetColorU32(ImVec4(0, 0, 0, 150)), true);
			DrawShadowedTexts(ImGui::GetBackgroundDrawList(), dataText, pos3, 15.0f, color, true);
		}

		if (Cheats::Esp::healthBar || Cheats::Esp::armorBar) {
			PedBarFix fix;
			fix.id = ped.getID();
			fix.health = ped.health;
			fix.armor = ped.armor;

			bool exists = false;
			for (auto& item : pedBarFix) {
				if (item.id == ped.getID()) {
					if (ped.health > 0) {
						item.health = ped.health;
					}
					else {
						ped.health = item.health;
					}

					if (ped.armor > 0) {
						item.health = ped.armor;
					}
					else {
						ped.armor = item.armor;
					}
					exists = true;
					break;
				}
			}

			if (!exists) {
				pedBarFix.push_back(fix);
			}
		}

		if (Cheats::Esp::healthBar || Cheats::Esp::armorBar) {
			PedBarFix fix;
			fix.id = ped.getID();
			fix.health = ped.health;
			fix.armor = ped.armor;
			bool exists = false;
			for (auto& item : pedBarFix) {
				if (item.id == ped.getID()) {
					if (ped.health > 0) item.health = ped.health; else ped.health = item.health;
					if (ped.armor > 0) item.armor = ped.armor; else ped.armor = item.armor;
					exists = true;
					break;
				}
			}
			if (!exists) pedBarFix.push_back(fix);
		}

		if (Cheats::Esp::healthBar) {
			float hFactor = pHeight / 100.0f;
			float hFull = hFactor * 100.0f;
			float hPerc = hFactor * (ped.health - 100.0f);
			float halfH = ped.health / 2.0f;
			ImVec4 colB(0.2f, 1.0f, 0.2f, 1.0f), colT(0.0f, 0.6f, 0.0f, 1.0f);
			ImU32 bg = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1));
			if (halfH != 0) {
				ImVec2 pos(pBase.x + reducedWidth + 5.0f, pBottom);
				float barW = 1.0f;
				drawHealthBar(drawList, pos, ImVec2(barW, hFull), bg);
				for (float i = 0; i < hPerc; i++) {
					float t = 1.0f - (i / hPerc);
					ImVec4 c(
						colB.x + (colT.x - colB.x) * t,
						colB.y + (colT.y - colB.y) * t,
						colB.z + (colT.z - colB.z) * t,
						1.0f
					);
					drawList->AddRectFilled(
						ImVec2(pos.x, pos.y - i),
						ImVec2(pos.x + barW, pos.y - i - 1),
						ImGui::ColorConvertFloat4ToU32(c)
					);
				}
				std::string tHealth = std::to_string((int)ped.health) + "%";
				ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(8.0f, FLT_MAX, 0.0f, tHealth.c_str());
				ImVec2 tPos(pos.x + barW + 5, pos.y - hFull - ts.y);
 			}
		}

		if (Cheats::Esp::armorBar) {
			ImVec4 colB(0.5f, 0.8f, 1.0f, 1.0f), colT(0.0f, 0.4f, 0.8f, 1.0f);
			ImU32 bg = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1));
			ImU32 outline = ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1));
			ImVec2 pos(pBase.x - reducedWidth - 5.0f, pBottom);
			float barW = 1.0f;
			float barH = pHeight;
			float aPerc = (ped.armor / 100.0f) * barH;
			drawHealthBar(drawList, pos, ImVec2(barW, barH), bg);
			for (float i = 0; i < aPerc; i++) {
				float t = 1.0f - (i / aPerc);
				ImVec4 c(
					colB.x + (colT.x - colB.x) * t,
					colB.y + (colT.y - colB.y) * t,
					colB.z + (colT.z - colB.z) * t,
					1.0f
				);
				drawList->AddRectFilled(
					ImVec2(pos.x, pos.y - i),
					ImVec2(pos.x + barW, pos.y - i - 1),
					ImGui::ColorConvertFloat4ToU32(c)
				);
			}
			std::string tArmor = std::to_string((int)ped.armor) + "%";
			ImVec2 ts = ImGui::GetFont()->CalcTextSizeA(8.0f, FLT_MAX, 0.0f, tArmor.c_str());
			ImVec2 tPos(pos.x + barW + 5, pos.y - barH - ts.y);
 		}


	 
		 
		if (Cheats::Esp::directionEsp) {
			ImColor color = ConvertToImColor(Cheats::Esp::directionEspColor);
			if (!ped.isVisible()) {
				color = DarkenColor(color, 0.3f);
			}
			ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
			ImVec2 directionToPed = ImVec2(pBase.x - screenCenter.x, pBase.y - screenCenter.y);
			float length = sqrt(directionToPed.x * directionToPed.x + directionToPed.y * directionToPed.y);
			directionToPed.x /= length;
			directionToPed.y /= length;

 			static float animationTime = 0.0f;
			animationTime += ImGui::GetIO().DeltaTime * 0.10f;  
			if (animationTime > 1.0f)
				animationTime = 0.0f;

 			float arrowDistanceFromCenter = 100.0f + sin(animationTime * 3.14f) * 10.0f; 
			ImVec2 arrowPos = ImVec2(screenCenter.x + directionToPed.x * arrowDistanceFromCenter, screenCenter.y + directionToPed.y * arrowDistanceFromCenter);
			ImVec2 arrowEndPos = ImVec2(screenCenter.x + directionToPed.x * (arrowDistanceFromCenter + 20.0f), screenCenter.y + directionToPed.y * (arrowDistanceFromCenter + 20.0f));
 			ImColor animatedColor = color;
			animatedColor.Value.w = 0.8f + sin(animationTime * 6.28f) * 0.2f;  
 			DrawArrowTriangleOutlined(drawList, arrowPos, arrowEndPos, animatedColor, 2.0f);
		}
	}
}


namespace vehicle_commands {
	struct VehiclePool {
		uint64_t* m_pListAddr;
		uint32_t* m_pBitArray;
		uint32_t m_Size;
		uint32_t _pad;
		uint32_t m_ItemSize;

		inline bool IsValid(uint32_t i) const {
			return (m_pBitArray[i >> 5] >> (i & 0x1F)) & 1;
		}
		inline uint64_t GetAddress(uint32_t i) const {
			return m_pListAddr[i];
		}
	};

	static VehiclePool** g_ppVehiclePool = nullptr;
	static uint32_t g_doorLockStateOffset = 0;
	static bool g_scanCompleted = false;

	inline bool PatternCompare(const uint8_t* data, const uint8_t* pattern, const char* mask) {
		for (; *mask; ++mask, ++data, ++pattern) {
			if (*mask == 'x' && *data != *pattern)
				return false;
		}
		return true;
	}

	inline uintptr_t FindPattern(const uint8_t* pattern, const char* mask) {
		uintptr_t base = Offsets.GameBase;
		size_t patLen = strlen(mask);

		for (size_t i = 0; i < 0x6000000 - patLen; ++i) {
			if (PatternCompare((uint8_t*)(base + i), pattern, mask))
				return base + i;
		}
		return 0;
	}

	inline uintptr_t ResolveRipRelative(uintptr_t addr, int operandOffset, int instructionLength) {
		int32_t rel = *(int32_t*)(addr + operandOffset);
		return addr + instructionLength + rel;
	}

	inline void ScanVehiclePatterns() {
		if (g_scanCompleted) return;

		
		static const uint8_t pool_pattern[] = {
			0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00,
			0xF3, 0x0F, 0x59, 0xF6,
			0x48, 0x8B, 0x0C, 0xD0,
			0x48, 0x85, 0xC9
		};
		static const char pool_mask[] = "xxx????xxxxxxxxxxx";

		uintptr_t poolHit = FindPattern(pool_pattern, pool_mask);
		if (poolHit) {
			uintptr_t resolved = ResolveRipRelative(poolHit, 3, 7);
			g_ppVehiclePool = (VehiclePool**)resolved;
		}

		
		static const uint8_t offset_pattern[] = {
			0x48, 0x85, 0xC0,
			0x74, 0x00,
			0x0F, 0xB6, 0x80, 0x00, 0x00, 0x00, 0x00,
			0x48, 0x8B, 0x5C, 0x24
		};
		static const char offset_mask[] = "xxxx?xxx????xxxx";

		uintptr_t offsetHit = FindPattern(offset_pattern, offset_mask);
		if (offsetHit)
			g_doorLockStateOffset = *(uint32_t*)(offsetHit + 8);

		if (g_ppVehiclePool && g_doorLockStateOffset)
			g_scanCompleted = true;
	}

	inline int UnlockAllVehicles() {
		if (!g_scanCompleted) {
			ScanVehiclePatterns();
			if (!g_scanCompleted) return 0;
		}

		VehiclePool* pool = *g_ppVehiclePool;
		if (!pool) return 0;

		int unlocked = 0;
		for (uint32_t i = 0; i < pool->m_Size; ++i) {
			if (!pool->IsValid(i)) continue;

			uint64_t vehAddr = pool->GetAddress(i);
			if (!vehAddr) continue;

			
			*(uint32_t*)(vehAddr + g_doorLockStateOffset) = 1;
			++unlocked;
		}
		return unlocked;
	}
}

void vehicleEsp() {
	if (!Cheats::Vehicle::vehicleEsp)
		return;

	uintptr_t localplayer = ReadMemory<uintptr_t>(gameWorld + 0x8);
	uintptr_t replayInterface = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.ReplayInterface);

	if (replayInterface) {
		uintptr_t vehicleInterface = ReadMemory<DWORD64>(replayInterface + 0x10);
		uintptr_t vehicleList = ReadMemory<DWORD64>(vehicleInterface + 0x180);
		int vehicleListCount = ReadMemory<int>(vehicleInterface + 0x188);

		for (int i = 0; i < vehicleListCount; ++i) {
			uintptr_t vehicle = ReadMemory<uintptr_t>(vehicleList + (i * 0x10));
			if (!vehicle) continue;

			float vehicleHealth = ReadMemory<float>(vehicle + Offsets.Health);
			float vehicleMaxHealth = 1000;
		 
			Vector3 getCordLocal = ReadMemory<Vector3>(localplayer + 0x90);
			Matrix viewMatrix = ReadMemory<Matrix>(viewPort + 0x24C);

			Vector3 getCorrdinateVehicle = ReadMemory<Vector3>(vehicle + 0x90);
			Vector2 vehicleLocation;

			if (!worldToScreen(viewMatrix, getCorrdinateVehicle, vehicleLocation))
				continue;

			Vector3 distanceCalculation = getCordLocal - getCorrdinateVehicle;
			double Distance = sqrtf(distanceCalculation.x * distanceCalculation.x + distanceCalculation.y * distanceCalculation.y + distanceCalculation.z * distanceCalculation.z);

			if (Distance <= Cheats::Vehicle::distance) {
				if ((int)Distance == 0 && !Cheats::Vehicle::drawLocalVehicle)
					continue;

				if ((int)Distance >= 1 && !Cheats::Vehicle::drawEnemyVehicle)
					continue;

				if (vehicleLocation.x == 0 || vehicleLocation.y == 0)
					continue;

				if (Cheats::Vehicle::vehicleHealt) {			 
					float healthPercentage = vehicleHealth / vehicleMaxHealth;
					float barWidth = 50.0f;   
					float barHeight = 5.0f;   
					float filledWidth = barWidth * healthPercentage;
					 
					ImColor healthColor = ImColor(255 - (int)(255 * healthPercentage), (int)(255 * healthPercentage), 0, 255);
					ImColor backgroundColor = ImColor(0, 0, 0, 255);				 
					ImVec2 barPosition(vehicleLocation.x - barWidth / 2, vehicleLocation.y - 20);
					ImVec2 barFilled(barPosition.x + filledWidth, barPosition.y + barHeight);

					ImGui::GetBackgroundDrawList()->AddRectFilled(barPosition, ImVec2(barPosition.x + barWidth, barPosition.y + barHeight), backgroundColor);
					ImGui::GetBackgroundDrawList()->AddRectFilled(barPosition, barFilled, healthColor);
				}

				if (Cheats::Vehicle::vehicleEspShowDistance) {
					std::string vehicleDistanceText = std::to_string((int)Distance) + " m";
					DrawShadowedTexts(ImGui::GetBackgroundDrawList(), vehicleDistanceText, ImVec2(vehicleLocation.x, vehicleLocation.y + 20), 14.f, ImGui::GetColorU32(ImVec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f)), true);
				}

				if (Cheats::Vehicle::vehicleEspSnapline) {
					ImGui::GetBackgroundDrawList()->AddLine(ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2, 0), ImVec2(vehicleLocation.x, vehicleLocation.y), ImGui::ColorConvertFloat4ToU32(ImColor(255, 255, 255)), 1.f);
				}

				if (Cheats::Vehicle::vehicleMarker) {
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(vehicleLocation.x, vehicleLocation.y + 45), 4, ImColor(0, 0, 0, 100), 100, 3.0f);
					ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(vehicleLocation.x, vehicleLocation.y + 45), 2, ImColor(255, 0, 0, 255), 100, 3.0f);
				}
			}
		}
	}
}

void UpdatePeds() {
	while (exitLoop == false) {
		std::vector<Ped> updatedPedList;
		gameWorld = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.GameWorld);
		localPlayer.pointer = ReadMemory<uintptr_t>(gameWorld + Offsets.LocalPlayer);
		viewPort = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.ViewPort);

		uintptr_t replayInterface = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.ReplayInterface);
		uintptr_t entityListPtr = ReadMemory<uintptr_t>(replayInterface + 0x18);
		uintptr_t entityList = ReadMemory<uintptr_t>(entityListPtr + 0x100);

		for (int i = 0; i < 256; i++) {
			Ped ped{};
			uintptr_t player = ReadMemory<uintptr_t>(entityList + (i * 0x10));
			if (player == localPlayer.pointer) {
				continue;
			}
			else if (!ped.getPlayer(player)) {
				continue;
			}
			else if (!ped.update()) {
				continue;
			}
			updatedPedList.push_back(ped);
		}
		pedList = updatedPedList;
		Sleep(500);
	}
}

 uint32_t CalCulateOffsets(uint64_t currentAddress, uint64_t targetAddress, int offset = 0x000000000000000A + 0x0000000000000205 + 0x0000000000000805 - 0x0000000000000A0F) {
	intptr_t relativeOffset = static_cast<intptr_t>(targetAddress - (currentAddress + offset));
	return static_cast<uint32_t>(relativeOffset);
}

void RestoreSilent() {
	std::vector<uint8_t> ReWriteTable =
	{
		0xF3, 0x41, 0x0F, 0x10, 0x19,
		0xF3, 0x41, 0x0F, 0x10, 0x41, 0x04,
		0xF3, 0x41, 0x0F, 0x10, 0x51, 0x08
	};

	WriteBytes(Offsets.GameBase + Offsets.Silent, &ReWriteTable[0], ReWriteTable.size());
	std::vector<uint8_t> AngleReWriteTable =
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
	};
	WriteBytes(Offsets.GameBase + 0x34E, &AngleReWriteTable[0], AngleReWriteTable.size());
}

Vector3 EndBulletPos;
void ApplySilent() {
	static uint64_t HandleBulletAddress = Offsets.GameBase + Offsets.Silent;
	static uint64_t AllocPtr = Offsets.GameBase + 0x34E;

	auto CalculateRelativeOffset = [](uint64_t CurrentAddress, uint64_t TargetAddress, int Offset = 5) {
			intptr_t RelativeOffset = static_cast<intptr_t>(TargetAddress - (CurrentAddress + Offset));
			return static_cast<uint32_t>(RelativeOffset);
	};

	union
	{
		float f;
		uint32_t i;
	} EndPosX, EndPosY, EndPosZ;

	EndPosX.f = EndBulletPos.x;
	EndPosY.f = EndBulletPos.y;
	EndPosZ.f = EndBulletPos.z;

	{
		std::vector<uint8_t> ReWriteTable =
		{
			0xE9, 0x00, 0x00, 0x00, 0x00
		};

		uint32_t JmpOffset = CalculateRelativeOffset(HandleBulletAddress, AllocPtr);
		ReWriteTable[1] = static_cast<uint8_t>(JmpOffset & 0xFF);
		ReWriteTable[2] = static_cast<uint8_t>((JmpOffset >> 8) & 0xFF);
		ReWriteTable[3] = static_cast<uint8_t>((JmpOffset >> 16) & 0xFF);
		ReWriteTable[4] = static_cast<uint8_t>((JmpOffset >> 24) & 0xFF);
		WriteBytes(HandleBulletAddress, &ReWriteTable[0], ReWriteTable.size());
	}

	{
		uintptr_t currentAddress = (uintptr_t)AllocPtr;
		uintptr_t targetAddress = (uintptr_t)(HandleBulletAddress);
		intptr_t relativeOffset = static_cast<intptr_t>(targetAddress - (currentAddress + 28));
		uint32_t jmpOffset = static_cast<uint32_t>(relativeOffset);

 		std::vector<uint8_t> ReWriteTable =
		{
			0x41, 0xC7, 0x01, static_cast<uint8_t>(EndPosX.i), static_cast<uint8_t>(EndPosX.i >> 8), static_cast<uint8_t>(EndPosX.i >> 16), static_cast<uint8_t>(EndPosX.i >> 24),
			0x41, 0xC7, 0x41, 0x04, static_cast<uint8_t>(EndPosY.i), static_cast<uint8_t>(EndPosY.i >> 8), static_cast<uint8_t>(EndPosY.i >> 16), static_cast<uint8_t>(EndPosY.i >> 24),
			0x41, 0xC7, 0x41, 0x08, static_cast<uint8_t>(EndPosZ.i), static_cast<uint8_t>(EndPosZ.i >> 8), static_cast<uint8_t>(EndPosZ.i >> 16), static_cast<uint8_t>(EndPosZ.i >> 24),
			0xF3, 0x41, 0x0F, 0x10, 0x19,
			0xE9, 0x00, 0x00, 0x00, 0x00
		};

 		ReWriteTable[29] = static_cast<uint8_t>(jmpOffset & 0xFF);
		ReWriteTable[30] = static_cast<uint8_t>((jmpOffset >> 8) & 0xFF);
		ReWriteTable[31] = static_cast<uint8_t>((jmpOffset >> 16) & 0xFF);
		ReWriteTable[32] = static_cast<uint8_t>((jmpOffset >> 24) & 0xFF);
		WriteBytes(AllocPtr, &ReWriteTable[0], ReWriteTable.size());
	}
}


namespace ws_silent_aim_bypass {
	inline uintptr_t g_remotePage = 0;
	inline bool g_initialized = false;
	inline std::vector<uint8_t> g_origBytes;

	inline bool initialize() {
		if (g_initialized) return true;

		
		uintptr_t impactAddr = Offsets.GameBase + Offsets.Silent;
		if (!impactAddr) return false;

		
		g_remotePage = (uintptr_t)VirtualAllocEx(Game.hProcess, NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!g_remotePage) return false;

		
		g_origBytes.resize(64);
		ReadProcessMemory(Game.hProcess, (LPCVOID)impactAddr, g_origBytes.data(), 64, NULL);

		g_initialized = true;
		return true;
	}

	inline void apply_shellcode(uintptr_t nativeAddr, uintptr_t dataPtr) {
		std::vector<uint8_t> code = { 0x48, 0xB8 }; 
		for (int i = 0; i < 8; i++) code.push_back(((uint8_t*)&dataPtr)[i]);

		code.insert(code.end(), { 0x48, 0x85, 0xD2 }); 
		code.insert(code.end(), { 0x74, 0x1E });         

		
		code.insert(code.end(), { 0xF3, 0x0F, 0x10, 0x00 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x11, 0x02 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x10, 0x40, 0x04 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x11, 0x42, 0x04 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x10, 0x40, 0x08 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x11, 0x42, 0x08 }); 

		
		code.insert(code.end(), { 0xB8, 0x01, 0x00, 0x00, 0x00 }); 
		code.insert(code.end(), { 0xC3 }); 

		WriteProcessMemory(Game.hProcess, (LPVOID)nativeAddr, code.data(), code.size(), NULL);
	}

	inline void restore() {
		if (!g_initialized) return;
		uintptr_t impactAddr = Offsets.GameBase + Offsets.Silent;
		WriteProcessMemory(Game.hProcess, (LPVOID)impactAddr, g_origBytes.data(), g_origBytes.size(), NULL);
	}

	inline void begin_shot(float tx, float ty, float tz) {
		if (!initialize()) return;

		float impact[3] = { tx, ty, tz };
		WriteProcessMemory(Game.hProcess, (LPVOID)g_remotePage, impact, 12, NULL);

		apply_shellcode(Offsets.GameBase + Offsets.Silent, g_remotePage);
	}

	inline void end_shot() { restore(); }

	inline bool is_available() { return g_initialized; }
	inline void shutdown() { restore(); g_initialized = false; }
}


namespace ws_aimbot_bypass {
	inline uintptr_t g_remotePage = 0;
	inline bool g_initialized = false;
	inline std::vector<uint8_t> g_origBytes;
	inline uintptr_t g_cameraAngleAddr = 0;

	inline bool initialize() {
		if (g_initialized) return true;

		
		g_cameraAngleAddr = Offsets.GameBase + Offsets.Camera;
		if (!g_cameraAngleAddr) return false;

		
		g_remotePage = (uintptr_t)VirtualAllocEx(Game.hProcess, NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!g_remotePage) return false;

		
		g_origBytes.resize(64);
		ReadProcessMemory(Game.hProcess, (LPCVOID)g_cameraAngleAddr, g_origBytes.data(), 64, NULL);

		g_initialized = true;
		return true;
	}

	inline void apply_shellcode(uintptr_t nativeAddr, uintptr_t dataPtr) {
		std::vector<uint8_t> code = { 0x48, 0xB8 }; 
		for (int i = 0; i < 8; i++) code.push_back(((uint8_t*)&dataPtr)[i]);

		code.insert(code.end(), { 0x48, 0x85, 0xD2 }); 
		code.insert(code.end(), { 0x74, 0x1E });         

		
		code.insert(code.end(), { 0xF3, 0x0F, 0x10, 0x00 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x11, 0x02 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x10, 0x40, 0x04 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x11, 0x42, 0x04 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x10, 0x40, 0x08 }); 
		code.insert(code.end(), { 0xF3, 0x0F, 0x11, 0x42, 0x08 }); 

		
		code.insert(code.end(), { 0xB8, 0x01, 0x00, 0x00, 0x00 }); 
		code.insert(code.end(), { 0xC3 }); 

		WriteProcessMemory(Game.hProcess, (LPVOID)nativeAddr, code.data(), code.size(), NULL);
	}

	inline void restore() {
		if (!g_initialized) return;
		WriteProcessMemory(Game.hProcess, (LPVOID)g_cameraAngleAddr, g_origBytes.data(), g_origBytes.size(), NULL);
	}

	inline void begin_aim(float pitch, float yaw, float roll) {
		if (!initialize()) return;

		float angles[3] = { pitch, yaw, roll };
		WriteProcessMemory(Game.hProcess, (LPVOID)g_remotePage, angles, 12, NULL);

		apply_shellcode(g_cameraAngleAddr, g_remotePage);
	}

	inline void end_aim() { restore(); }

	inline bool is_available() { return g_initialized; }
	inline void shutdown() { restore(); g_initialized = false; }
}


namespace ws_trigger_bypass {
	inline bool g_initialized = false;
	inline uintptr_t g_remotePage = 0;

	inline bool initialize() {
		if (g_initialized) return true;

		
		g_remotePage = (uintptr_t)VirtualAllocEx(Game.hProcess, NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!g_remotePage) return false;

		g_initialized = true;
		return true;
	}

	inline void apply_click_shellcode(uintptr_t nativeAddr, uintptr_t dataPtr) {
		std::vector<uint8_t> code = { 0x48, 0xB8 }; 
		for (int i = 0; i < 8; i++) code.push_back(((uint8_t*)&dataPtr)[i]);

		
		code.insert(code.end(), { 0x8B, 0x00 });           
		code.insert(code.end(), { 0x85, 0xC0 });           
		code.insert(code.end(), { 0x74, 0x06 });           

		
		code.insert(code.end(), { 0xB8, 0x01, 0x00, 0x00, 0x00 }); 
		code.insert(code.end(), { 0xC3 }); 

		
		code.insert(code.end(), { 0x31, 0xC0 }); 
		code.insert(code.end(), { 0xC3 }); 

		WriteProcessMemory(Game.hProcess, (LPVOID)nativeAddr, code.data(), code.size(), NULL);
	}

	inline void trigger_click(bool state) {
		if (!initialize()) return;

		int clickState = state ? 1 : 0;
		WriteProcessMemory(Game.hProcess, (LPVOID)g_remotePage, &clickState, sizeof(int), NULL);
	}

	inline void shutdown() { g_initialized = false; }
}

Ped FindBestTarget(int aimFov) {
	Ped bestTarget;
	float minFov = 9999.f;
	for (auto& ped : pedList) {
		if (!localPlayer.update()) {
			break;
		}

		if (!ped.update()) {
			continue;
		}

		float pDistance = getDistance(ped.position, localPlayer.position);
		if (pDistance >= Cheats::Aim::distance)
			continue;

		if (Cheats::Esp::onlyVisible && !ped.isVisible())
			continue;

		if (Cheats::Aim::ignorePed && !ped.isPlayer())
			continue;

		if (Cheats::Aim::ignoreDeath && ped.isDead())
			continue;

		Vector2 screenPosition;
		Matrix viewMatrix = ReadMemory<Matrix>(viewPort + 0x24C);
		if (Cheats::Aim::aimbotSelectedType == 0) {
			if (!worldToScreen(viewMatrix, ped.boneList[Head], screenPosition)) {
				continue;
			}
		}
		else if (Cheats::Aim::aimbotSelectedType == 1) {
			if (!worldToScreen(viewMatrix, ped.boneList[Hip], screenPosition)) {
				continue;
			}
		}else if (Cheats::Aim::aimbotSelectedType == 2) {
			if (!worldToScreen(viewMatrix, ped.boneList[Neck], screenPosition)) {
				continue;
			}
		}

		float fov = abs((Vector2(Game.lpRect.right / 2.f, Game.lpRect.bottom / 2.f) - screenPosition).Length());
		if (fov < aimFov) {
			if (fov < minFov) {
				bestTarget = ped;
				minFov = fov;
				continue;
			}
		}
	}
	return bestTarget;
}
 
void SetAim() {
	while (exitLoop == false) {
		if (Cheats::Aim::aimBot) {
			Ped target = FindBestTarget(Cheats::Aim::aimBotFov);
			if (GetAsyncKeyState(Cheats::Aim::aimBotKey) & 0x8000) {
				if (!vec3Empty(target.boneList[Head])) {
					uintptr_t camera = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.Camera);
					Vector3 viewAngle = ReadMemory<Vector3>(camera + 0x3D0);
					Vector3 cameraPosition = ReadMemory<Vector3>(camera + 0x60);
					Vector3 angle;
					if (Cheats::Aim::aimbotSelectedType == 0) {
						if (vec3Empty(target.boneList[Head])) {
							continue;
						}
						angle = calcAngle(cameraPosition, target.boneList[Head]);
					}
					else if (Cheats::Aim::aimbotSelectedType == 1) {
						if (vec3Empty(target.boneList[Hip])) {
							continue;
						}
						angle = calcAngle(cameraPosition, target.boneList[Hip]);
					}
					else if (Cheats::Aim::aimbotSelectedType == 2) {
						if (vec3Empty(target.boneList[Neck])) {
							continue;
						}
						angle = calcAngle(cameraPosition, target.boneList[Neck]);
					}
					 

					normalizeAngles(angle);
					Vector3 delta = angle - viewAngle;
					normalizeAngles(delta);
					Vector3 writeAngle = viewAngle + (Cheats::Aim::aimBotSmooth ? delta / Cheats::Aim::aimBotSmooth : delta);
					normalizeAngles(writeAngle);
					if (!vec3Empty(writeAngle)) {
						
						ws_aimbot_bypass::begin_aim(writeAngle.x, writeAngle.y, writeAngle.z);
						WriteMemory<Vector3>(camera + 0x3D0, writeAngle);
						ws_aimbot_bypass::end_aim();
					}
				}
			}
		}
		if (Cheats::Aim::silent) {
			static bool initialized = false;
			if (!initialized) {
				srand(static_cast<unsigned>(time(0)));
				ws_silent_aim_bypass::initialize(); 
				initialized = true;
			}
			
			HANDLE x9c;
 			std::vector<int> boneTypes = { Hip, Hip, Neck, Hip, Hip, Neck, Hip, Hip, Neck, Hip, Hip, Neck };
			int randomIndex = rand() % boneTypes.size();
			Ped target = FindBestTarget(Cheats::Aim::silentFov);
			Vector3 SlientType;
			uintptr_t camera = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.Camera);
			Vector3 viewAngle = ReadMemory<Vector3>(camera + 0x3D0);
			Vector3 cameraPosition = ReadMemory<Vector3>(camera + 0x60); 
			if (Cheats::Aim::SlientSlectedType == 0) {
				SlientType = target.boneList[Head];
			}
			else if (Cheats::Aim::SlientSlectedType == 1) {
				SlientType = target.boneList[Hip];
			}
			else if (Cheats::Aim::SlientSlectedType == 2) {
				SlientType = target.boneList[boneTypes[randomIndex]];
			}
			else if (Cheats::Aim::SlientSlectedType == 3) {
				SlientType = target.boneList[Neck];
			}

			float distanceToTarget = getDistance(localPlayer.position, target.position);
			if (distanceToTarget < 5.0f) {
				SlientType = target.boneList[boneTypes[randomIndex]];
			}
			if (vec3Empty(SlientType)) {
				ws_silent_aim_bypass::end_shot();
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RestoreSilent, NULL, NULL, NULL);
			}
			else {
				Vector3 angle = calcAngle(cameraPosition, SlientType);
				Vector3 delta = angle - viewAngle;
				normalizeAngles(delta);
				float fovDistance = sqrtf(delta.x * delta.x + delta.y * delta.y);
				if (fovDistance <= Cheats::Aim::silentFov) {
					if (GetAsyncKeyState(Cheats::Aim::silentKey) & 0x8000) {
						EndBulletPos = SlientType;
						
						
						ws_silent_aim_bypass::begin_shot(SlientType.x, SlientType.y, SlientType.z);
						
						if (Cheats::Aim::Pslient) {
							x9c = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RestoreSilent, NULL, 0, NULL);
							x9c = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ApplySilent, NULL, 0, NULL);
							x9c = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RestoreSilent, NULL, 0, NULL);
							CloseHandle(x9c);
							CloseHandle(x9c);
							CloseHandle(x9c);
 						}
						else {
							x9c = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ApplySilent, NULL, 0, NULL);
							CloseHandle(x9c);
 						}
						
						
						ws_silent_aim_bypass::end_shot();
					}
				}
				else {
					ws_silent_aim_bypass::end_shot();
					x9c = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RestoreSilent, NULL, 0, NULL);
					CloseHandle(x9c);
				}
			}
		}
		Sleep(1);
	}
}

bool isTargetInCrosshair(const Vector2& screenPosition) {
	const float crosshairX = (float)Game.lpRect.right / 2;
	const float crosshairY = (float)Game.lpRect.bottom / 2;
	return (abs(screenPosition.x - crosshairX) <= Cheats::Aim::crosshairTolerance && abs(screenPosition.y - crosshairY) <= Cheats::Aim::crosshairTolerance);
}

void shoot() {
	
	ws_trigger_bypass::trigger_click(true);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); 
	Sleep(Cheats::Aim::triggerDelay);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	ws_trigger_bypass::trigger_click(false);
}

void TriggerBot() {
	while (exitLoop == false) {
		if (Cheats::Aim::triggerBot && (GetAsyncKeyState(Cheats::Aim::triggerKey) & 0x8000)) {
			Matrix viewMatrix = ReadMemory<Matrix>(viewPort + 0x24C);
			Ped target = FindBestTarget(Cheats::Aim::triggerFov);
			Vector3 TriggerBottype;
			if (Cheats::Aim::triggerBotSelectedType == 0) {
				TriggerBottype = target.boneList[Head];
			}
			else if (Cheats::Aim::triggerBotSelectedType == 1) {
				TriggerBottype = target.boneList[Hip];
			}
			else if (Cheats::Aim::triggerBotSelectedType == 2) {
				TriggerBottype = target.boneList[Neck];
 			}
			if (!vec3Empty(TriggerBottype)) {
				Vector2 screenPosition;
				if (worldToScreen(viewMatrix, TriggerBottype, screenPosition)) {
					if (isTargetInCrosshair(screenPosition)) {
						shoot();
					}
				}
			}
		}
		Sleep(1);  
	}
}

void TeleportObject(uintptr_t Object, uintptr_t Navigation, uintptr_t ModelInfo, Vector3 Position, Vector3 VisualPosition, bool Stop) {
	float BackupMagic = 0.f;
	if (Stop) {
		BackupMagic = ReadMemory<float>(ModelInfo + 0x2C);
		WriteMemory(ModelInfo + 0x2C, 0.f);
	}

	WriteMemory(Object + 0x90, VisualPosition);
	WriteMemory(Navigation + 0x50, Position);
	if (Stop) {
		std::this_thread::sleep_for(std::chrono::milliseconds(40));
		WriteMemory(ModelInfo + 0x2C, BackupMagic);
	}
}

void TeleportToWaypoint() {
	for (int i = 0; i < 2000; i++) {
		uint64_t Blip = ReadMemory<uint64_t>(Offsets.GameBase + Offsets.Waypoint + (i * 8));
		if (!Blip)
			continue;

		int BlipIcon = ReadMemory<int>(Blip + 0x40);
		int BlipColor = ReadMemory<int>(Blip + 0x48);

		if ((BlipColor != 84) || (BlipIcon != 8))
			continue;

		Vector2 WaypointPos = ReadMemory<Vector2>(Blip + 0x10);
		if (WaypointPos.x != 0 && WaypointPos.y != 0) {
			uint64_t Object = NULL;
			uint64_t Navigation = NULL;
			uint64_t ModelInfo = NULL;

			if (!Object || !Navigation || !ModelInfo) {
				Object = (uint64_t)localPlayer.pointer;
				Navigation = ReadMemory<uint64_t>(localPlayer.pointer + 0x30);
				ModelInfo = ReadMemory<uint64_t>(localPlayer.pointer + 0x20);
			}

			Vector3 TeleportPos = Vector3(WaypointPos.x, WaypointPos.y, -210.f);
			TeleportObject(Object, Navigation, ModelInfo, TeleportPos, TeleportPos, true);
		}
	}
}


namespace noclip_state {
	inline bool was_active = false;
	inline Vector3 last_position = { 0.0f, 0.0f, 0.0f };
	inline bool position_frozen = false;
}

void Noclip(Vector3 CameraPos) {
	if (!localPlayer.pointer)
		return;

	if (!Cheats::Misc::NoClip) {
		
		if (noclip_state::was_active) {
			
			uintptr_t entity = localPlayer.pointer;
			
			uintptr_t vehicle = ReadMemory<uintptr_t>(localPlayer.pointer + Offsets.Vehicle);
			if (vehicle) entity = vehicle;

			
			WriteMemory<int>(entity + 0x138, 1); 
			
			WriteMemory<float>(entity + 0x140, 9.8f); 

			noclip_state::was_active = false;
		}
		return;
	}

	
	if (!noclip_state::was_active) {
		noclip_state::was_active = true;
	}

	
	uintptr_t entity = localPlayer.pointer;
	uintptr_t vehicle = ReadMemory<uintptr_t>(localPlayer.pointer + Offsets.Vehicle);
	if (vehicle) entity = vehicle;

	
	WriteMemory<int>(entity + 0x138, 0); 
	WriteMemory<float>(entity + 0x140, 0.0f); 

	
	Vector3 currentPos = ReadMemory<Vector3>(entity + 0x90);
	Vector3 newPos = currentPos;

	
	float speed = static_cast<float>(Cheats::Misc::NoClipSpeed) / 10.0f;

	
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		speed *= 4.0f;

	
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
		speed *= 0.25f;

	
	float pitch = CameraPos.z * (3.14159f / 180.0f); 
	float yaw = CameraPos.x * (3.14159f / 180.0f);   

	
	Vector3 forward;
	forward.x = -sinf(yaw) * cosf(pitch);
	forward.y = cosf(yaw) * cosf(pitch);
	forward.z = sinf(pitch);

	
	Vector3 right;
	right.x = cosf(yaw);
	right.y = sinf(yaw);
	right.z = 0.0f;

	
	if (GetAsyncKeyState('W') & 0x8000) {
		newPos.x += forward.x * speed;
		newPos.y += forward.y * speed;
		newPos.z += forward.z * speed;
	}
	
	if (GetAsyncKeyState('S') & 0x8000) {
		newPos.x -= forward.x * speed;
		newPos.y -= forward.y * speed;
		newPos.z -= forward.z * speed;
	}
	
	if (GetAsyncKeyState('A') & 0x8000) {
		newPos.x -= right.x * speed;
		newPos.y -= right.y * speed;
	}
	
	if (GetAsyncKeyState('D') & 0x8000) {
		newPos.x += right.x * speed;
		newPos.y += right.y * speed;
	}
	
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		newPos.z += speed;
	}
	
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 && !(GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
		newPos.z -= speed;
	}

	
	WriteMemory<Vector3>(entity + 0x30, Vector3(0, 0, 0)); 

	
	WriteMemory<Vector3>(entity + 0x90, newPos);

	
	WriteMemory<float>(entity + 0x40, 0.0f); 
	WriteMemory<float>(entity + 0x44, 0.0f); 
	WriteMemory<float>(entity + 0x48, yaw);  

	noclip_state::last_position = newPos;
}

void MiscFeatures() {
	static bool lastState = false;   	 
	while (exitLoop == false) {
		uintptr_t weaponManager = ReadMemory<uintptr_t>(localPlayer.pointer + Offsets.WeaponManager);
		uintptr_t weaponinfo = ReadMemory<uintptr_t>(weaponManager + 0x20);
		
		TPModelInfo = ReadMemory<uintptr_t>(localPlayer.pointer + 0x20);
		TPPosition = ReadMemory<Vector3>(localPlayer.pointer + 0x90);
		TPNavigation = ReadMemory<uintptr_t>(localPlayer.pointer + 0x30);
		 
		uintptr_t camera = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.BlipList);
		uintptr_t camera2 = ReadMemory<uintptr_t>(camera + 0x3C0);
		Vector3 camerapos = ReadMemory<Vector3>(camera2 + 0x40);
		Noclip(camerapos);
		 
		if (Cheats::Misc::infiniteAmmo) {
			uintptr_t AmmoInfo = ReadMemory<uintptr_t>(weaponinfo + 0x60);
			uintptr_t AmmoCount = ReadMemory<uintptr_t>(AmmoInfo + 0x8);
			uintptr_t AmmoCount2 = ReadMemory<uintptr_t>(AmmoCount + 0x0);
			WriteMemory<float>(AmmoCount2 + 0x18, 30);
		}
		if (Cheats::Misc::TeleportWaypoint) {
			TeleportToWaypoint();
			Cheats::Misc::TeleportWaypoint = false;
		}
		  
		if (Cheats::Misc::noRecoil) {
			 WriteMemory<float>(weaponinfo + 0x2F4, 0.25f);
		}	  
		 

		if (Cheats::Misc::noSpread) {
			WriteMemory<float>(weaponinfo + 0x84, 0.0f);
		}
		else {
			WriteMemory<float>(weaponinfo + 0x84, 1.0f);
		}

		if (Cheats::Misc::noReload) {
			WriteMemory<float>(weaponinfo + 0x134, 1000);
		}
		else {
			WriteMemory<float>(weaponinfo + 0x134, 1);

		}

		if (Cheats::Misc::noRange) {
			WriteMemory<float>(weaponinfo + 0x28C, 1000.f);
		}
		 
		
		{
			static uint32_t last_weapon = 0;
			static float original_damage_modifier = 1.0f;
			static float cached_weapon_damage = 0.0f;
			static bool was_damage_boost_active = false;

			if (Cheats::Misc::damageBoost) {
				
				uint32_t current_weapon = ReadMemory<uint32_t>(weaponinfo + 0x10); 
				
				
				if (last_weapon != current_weapon) {
					cached_weapon_damage = ReadMemory<float>(weaponinfo + 0xB0); 
					last_weapon = current_weapon;
				}
				
				float multiplier = (float)Cheats::Misc::damageValue / 10.0f; 
				float new_damage = cached_weapon_damage * multiplier;
				
				
				if (new_damage > 999.0f) new_damage = 999.0f;
				
				WriteMemory<float>(weaponinfo + 0xB0, new_damage);
				was_damage_boost_active = true;
			}
			else {
				
				if (was_damage_boost_active) {
					if (cached_weapon_damage > 0.0f) {
						WriteMemory<float>(weaponinfo + 0xB0, cached_weapon_damage);
					}
					was_damage_boost_active = false;
					last_weapon = 0;
				}
			}
		}

		if (Cheats::Misc::reloadAmmo) {
			if (GetAsyncKeyState(Cheats::Misc::reloadAmmoKey) & 0x8000) {
				uintptr_t AmmoInfo = ReadMemory<uintptr_t>(weaponinfo + 0x60);
				uintptr_t AmmoCount = ReadMemory<uintptr_t>(AmmoInfo + 0x8);
				uintptr_t AmmoCount2 = ReadMemory<uintptr_t>(AmmoCount + 0x0);
				WriteMemory<float>(AmmoCount2 + 0x18, Cheats::Misc::reloadValue);
			}
		}

		
		{
			static int last_health = -1;
			static int last_armor = -1;
			static int target_health = -1;
			static int target_armor = -1;
			static DWORD health_damage_time = 0;
			static DWORD armor_damage_time = 0;
			static bool health_pending = false;
			static bool armor_pending = false;
			static bool was_active = false;

			if (Cheats::Misc::godMode) {
				if (!was_active) {
					Beep(1000, 100);
					was_active = true;
				}

				if (!localPlayer.pointer) {
					last_health = -1;
					last_armor = -1;
					health_pending = false;
					armor_pending = false;
				}
				else {
					int current_health = ReadMemory<int>(localPlayer.pointer + 0x280);
					int current_armor = ReadMemory<int>(localPlayer.pointer + Offsets.Armor);
					int max_health = 200; 
					int max_armor = 100;  
					DWORD now = GetTickCount();

					int delay_ms = 3000; 

					
					if (last_health == -1) {
						last_health = current_health;
						last_armor = current_armor;
					}

					
					if (current_health < last_health) {
						
						if (!health_pending) {
							target_health = last_health;
							health_damage_time = now;
							health_pending = true;
						}
						else {
							
							health_damage_time = now;
						}
					}
					else if (current_health > last_health) {
						
						health_pending = false;
					}

					
					if (current_armor < last_armor) {
						if (!armor_pending) {
							target_armor = last_armor;
							armor_damage_time = now;
							armor_pending = true;
						}
						else {
							armor_damage_time = now;
						}
					}
					else if (current_armor > last_armor) {
						armor_pending = false;
					}

					
					if (health_pending) {
						if (now - health_damage_time >= (DWORD)delay_ms) {
							int restore_value = target_health;
							if (restore_value > max_health) restore_value = max_health;

							WriteMemory<int>(localPlayer.pointer + 0x280, restore_value);
							health_pending = false;
							current_health = restore_value;
						}
					}

					
					if (armor_pending) {
						if (now - armor_damage_time >= (DWORD)delay_ms) {
							int restore_value = target_armor;
							if (restore_value > max_armor) restore_value = max_armor;

							WriteMemory<int>(localPlayer.pointer + Offsets.Armor, restore_value);
							armor_pending = false;
							current_armor = restore_value;
						}
					}

					
					last_health = current_health;
					last_armor = current_armor;
				}
			}
			else {
				if (was_active) {
					Beep(500, 100);
					was_active = false;
				}
				
				last_health = -1;
				last_armor = -1;
				health_pending = false;
				armor_pending = false;
			}
		}
		 

		
		{
			static bool heal_was_pressed = false;
			
			if (Cheats::Misc::healthBoost) {
				bool is_pressed = (GetAsyncKeyState(Cheats::Misc::healthBoostKey) & 0x8000) != 0;
				
				
				if (is_pressed && !heal_was_pressed) {
					if (localPlayer.pointer && !ReadMemory<bool>(localPlayer.pointer + 0x189)) {
						float maxHealth = 200.0f; 
						float currentHealth = ReadMemory<float>(localPlayer.pointer + 0x280);
						if (currentHealth < maxHealth) {
							WriteMemory<float>(localPlayer.pointer + 0x280, maxHealth);
						}
					}
				}
				heal_was_pressed = is_pressed;
			}
		}

		
		{
			static bool armor_was_pressed = false;
			
			if (Cheats::Misc::armorBoost) {
				bool is_pressed = (GetAsyncKeyState(Cheats::Misc::armorBoostKey) & 0x8000) != 0;
				
				
				if (is_pressed && !armor_was_pressed) {
					if (localPlayer.pointer) {
						int targetArmor = Cheats::Misc::armorBoostValue; 
						WriteMemory<int>(localPlayer.pointer + Offsets.Armor, targetArmor);
					}
				}
				armor_was_pressed = is_pressed;
			}
		}

		if (Cheats::Misc::fovChanger) {
			uintptr_t camera = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.Camera);
			WriteMemory<float>(ReadMemory<uintptr_t>(camera + 0x10) + 0x30, Cheats::Misc::fovValue);
		}
		else {
			uintptr_t camera = ReadMemory<uintptr_t>(Offsets.GameBase + Offsets.Camera);
			WriteMemory<float>(ReadMemory<uintptr_t>(camera + 0x10) + 0x30, 50);
		}

		
		{
			static uintptr_t engineHealthAddr = 0;
			static std::vector<uint8_t> originalBytes;
			static bool isPatched = false;
			static bool initialized = false;

			if (!initialized) {
				
				const char* pattern = "\x48\x83\xEC\x00\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\x00\x48\x8D\x88\x00\x00\x00\x00\x48\x83\xC4\x00\xE9\x00\x00\x00\x00\xF3\x0F\x10\x05";
				const char* mask = "xxx?x????xxxx?xxx????xxx?x????xxxx";
				
				uintptr_t base = Offsets.GameBase;
				SIZE_T scanSize = 0x6000000;
				
				for (SIZE_T i = 0; i < scanSize; i++) {
					uintptr_t addr = base + i;
					bool found = true;
					
					for (SIZE_T j = 0; mask[j] != '\0'; j++) {
						if (mask[j] == 'x') {
							uint8_t byte = ReadMemory<uint8_t>(addr + j);
							if (byte != (uint8_t)pattern[j]) {
								found = false;
								break;
							}
						}
					}
					
					if (found) {
						engineHealthAddr = addr;
						
						originalBytes.resize(10);
						for (int k = 0; k < 10; k++) {
							originalBytes[k] = ReadMemory<uint8_t>(addr + k);
						}
						break;
					}
				}
				initialized = true;
			}

			if (Cheats::Misc::vehicleFix) {
				if (engineHealthAddr && !isPatched) {
					
					uint8_t patchBytes[] = { 0xB8, 0x00, 0x00, 0x7A, 0x44, 0x66, 0x0F, 0x6E, 0xC0, 0xC3 };
					WriteBytes(engineHealthAddr, patchBytes, sizeof(patchBytes));
					isPatched = true;
				}
			}
			else {
				if (engineHealthAddr && isPatched) {
					
					WriteBytes(engineHealthAddr, originalBytes.data(), originalBytes.size());
					isPatched = false;
				}
			}
		}

		
		if (Cheats::Misc::vehicleUnlock) {
			static bool unlockInitialized = false;
			if (!unlockInitialized) {
				vehicle_commands::ScanVehiclePatterns();
				unlockInitialized = true;
			}
			if (vehicle_commands::g_scanCompleted) {
				vehicle_commands::UnlockAllVehicles();
			}
		}

		Sleep(1);
	}
}


 