#pragma once
using namespace DirectX::SimpleMath;

struct GameStruct {
	HWND Window;
	HANDLE Handle;
	DWORD pID;
	std::string Path;
	std::string Version;
	std::string ServerIP;
	std::string ServerPort;
	bool MainVer = false;
}Game;

struct Offsets {
	uintptr_t GameBase;
	uintptr_t GameWorld;
	uintptr_t ReplayInterface;
	uintptr_t ViewPort;
	uintptr_t LocalPlayer;
	uintptr_t Camera;
	uintptr_t BlipList;
	uintptr_t PlayerInfo;
	uintptr_t Id;
	uintptr_t Health;
	uintptr_t MaxHealth;
	uintptr_t Armor;
	uintptr_t WeaponManager;
	uintptr_t BoneList;
	uintptr_t Silent;
	uint64_t Waypoint;
	uintptr_t Vehicle;
	uintptr_t VisibleFlag;
} offsets;

struct Player {
	int Id;
	std::string Username;
};

std::vector<Player> Players;

struct Target {
	DWORD pID;
	HWND hWnd;
	HANDLE hProcess;
	LPCSTR lpClassName = "grcWindow";
	LPCSTR lpWindowName;
	RECT lpRect;
	POINT lpPoint;
} target;

struct Overlay {
	HWND hWnd;
	WNDCLASSEX wndClassEx;
	LPCSTR lpClassName = "Spotify";
	LPCSTR lpWindowName = "Spotify";
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
} overlay;

void GetOffsets() {
	if (Game.Version == "3407") {
		Offsets.GameWorld = 0x25D7108; 
		Offsets.ReplayInterface = 0x1F9A9D8;
		Offsets.ViewPort = 0x20431C0;
		Offsets.Camera = 0x20440C8;
		Offsets.BlipList = 0x202EB48;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10A8;
		Offsets.Id = 0xE8;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x284;
		Offsets.Armor = 0x150C;
		Offsets.WeaponManager = 0x10B8;
		Offsets.BoneList = 0x410;
		Offsets.Silent = 0x102FF89;
		Offsets.Vehicle = 0x0D10;
		Offsets.VisibleFlag = 0x145C;
		Offsets.Waypoint = 0x2047D50;
	}
	if (Game.Version == "3323") {
		Offsets.GameWorld = 0x25C15B0; 
		Offsets.ReplayInterface = 0x1F85458;
		Offsets.ViewPort = 0x202DC50;
		Offsets.Camera = 0x202E878;
		Offsets.BlipList = 0x202EB48;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10A8;
		Offsets.Id = 0xE8;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x284;
		Offsets.Armor = 0x150C;
		Offsets.WeaponManager = 0x10B8;
		Offsets.BoneList = 0x410;
		Offsets.Silent = 0x1026CAD;
		Offsets.Vehicle = 0x0D10;
		Offsets.VisibleFlag = 0x145C;
		Offsets.Waypoint = 0x20333E0;
	}
	if (Game.Version == "3258") {
		Offsets.GameWorld = 0x25B14B0;
		Offsets.ReplayInterface = 0x1FBD4F0;
		Offsets.ViewPort = 0x201DBA0;
		Offsets.Camera = 0x201E7D0;
		Offsets.BlipList = 0x2002FA0;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10A8;
		Offsets.Id = 0xE8;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x284;
		Offsets.Armor = 0x150C;
		Offsets.WeaponManager = 0x10B8;
		Offsets.BoneList = 0x410;
		Offsets.Silent = 0x101A65D;
		Offsets.Vehicle = 0x0D10;
		Offsets.VisibleFlag = 0x145C;
		Offsets.Waypoint = 0x2023400;
	}
	if (Game.Version == "3095") {
		Offsets.GameWorld = 0x2593320;
		Offsets.ReplayInterface = 0x1F58B58;
		Offsets.ViewPort = 0x20019E0;
		Offsets.Camera = 0x20025B8;
		Offsets.BlipList = 0x2002888;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10A8;
		Offsets.Id = 0xE8;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x284;
		Offsets.Armor = 0x150C;
		Offsets.WeaponManager = 0x10B8;
		Offsets.BoneList = 0x410;
		Offsets.Silent = 0x100F5A4;
		Offsets.Vehicle = 0x0D10;
		Offsets.VisibleFlag = 0x145C;
		Offsets.Waypoint = 0x2002FA0;
	}
	if (Game.Version == "2944") {
		Offsets.GameWorld = 0x257BEA0;
		Offsets.ReplayInterface = 0x1F42068;
		Offsets.ViewPort = 0x1FEAAC0;
		Offsets.Camera = 0x1FEB968;
		Offsets.BlipList = 0x1FEB968;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10A8;
		Offsets.Id = 0xE8;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x284;
		Offsets.Armor = 0x150C;
		Offsets.WeaponManager = 0x10B8;
		Offsets.BoneList = 0x410;
		Offsets.Silent = 0x1003F80;
		Offsets.VisibleFlag = 0x145C;
		Offsets.Waypoint = 0x1FF3130;

	}
	if (Game.Version == "2802") {
		Offsets.GameWorld = 0x254D448;
		Offsets.ReplayInterface = 0x1F5B820;
		Offsets.ViewPort = 0x1FBC100;
		Offsets.Camera = 0x1FBCCD8;
		Offsets.BlipList = 0x1FBCFA8;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10A8;
		Offsets.Id = 0xE8;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x284;
		Offsets.Armor = 0x150C;
		Offsets.WeaponManager = 0x10B8;
		Offsets.BoneList = 0x410;
		Offsets.Silent = 0xFF716C;
		Offsets.VisibleFlag = 0x145C;
		Offsets.Waypoint = 0x1FBD6E0;

	}
	if (Game.Version == "2699") {
		Offsets.GameWorld = 0x26684D8;
		Offsets.ReplayInterface = 0x20304C8;
		Offsets.ViewPort = 0x20D8C90;
		Offsets.Camera = 0x20D9868;
		Offsets.BlipList = 0x20D9B38;
		Offsets.LocalPlayer = 0x8;
		Offsets.PlayerInfo = 0x10C8;
		Offsets.Id = 0x88;
		Offsets.Health = 0x280;
		Offsets.MaxHealth = 0x2A0;
		Offsets.Armor = 0x1530;
		Offsets.WeaponManager = 0x10D8;
		Offsets.BoneList = 0x430;
		Offsets.Silent = 0xFF9D90;
		Offsets.VisibleFlag = 0x147C;
		Offsets.Waypoint = 0x20E1420;
	}
}