#pragma once
#include <windows.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <list>
#include <algorithm>
#include "math.h"
#include "overlay.h"
#include "vector.h"
ImColor teamColor[] = {
	ImColor(0, 0, 0),          // 0: Doesn't exit 
	ImColor(0, 0, 0),          // 0: Spectator
	ImColor(17, 124, 141),     // 1: Tealish
	ImColor(33, 71, 103),      // 2: Dark Blue
	ImColor(31, 84, 205),      // 3: Blue
	ImColor(68, 42, 96),       // 4: Light Purple
	ImColor(110, 44, 111),     // 5: Light Green
	ImColor(173, 45, 119),     // 6: Light Magenta
	ImColor(175, 30, 81),      // 7: Green
	ImColor(194, 5, 15),       // 8: Red
	ImColor(197, 67, 32),      // 9: Pink
	ImColor(119, 32, 22),      // 10: Brownish Orange
	ImColor(158, 60, 16),      // 11: Orange
	ImColor(118, 75, 5),       // 12: Olive Green
	ImColor(170, 101, 34),     // 13: Yellowish Orange
	ImColor(150, 125, 0),      // 14: Yellow-Green
	ImColor(133, 147, 10),     // 15: Light Green
	ImColor(73, 88, 3),        // 16: Dark Green
	ImColor(107, 143, 66),     // 17: Light Olive
	ImColor(59, 131, 57),      // 18: Olive
	ImColor(47, 90, 26),       // 19: Brown
	ImColor(5, 116, 88),       // 20: Purple
	ImColor(110, 50, 6),       // 21: Bluish Green
	ImColor(14, 88, 106),      // 22: Sky Blue
	ImColor(10, 130, 148),     // 23: Bright Green
	ImColor(27, 71, 105),      // 24: Pale Blue
	ImColor(38, 84, 195),      // 25: Bright Blue
	ImColor(70, 48, 96),       // 26: Purple
	ImColor(110, 44, 111),     // 27: Light Green
	ImColor(173, 45, 119),     // 28: Light Magenta
	ImColor(176, 28, 81),      // 29: Light Red
	ImColor(195, 0, 11)        // 30: Bright Red
};

class HitBoxManager
{
private:
public:
	Vector ScreenHeadBone{};
	Vector ScreenNeckBone{};
	Vector ScreenChestBone{};
	Vector ScreenWaistBone{};
	Vector ScreenBotmBone{};

	Vector ScreenLeftshoulderBone{};
	Vector ScreenLeftelbowBone{};
	Vector ScreenLeftHandBone{};
	Vector ScreenRightshoulderBone{};
	Vector ScreenRightelbowBone{};
	Vector ScreenRightHandBone{};

	Vector ScreenLeftThighsBone{};
	Vector ScreenLeftkneesBone{};
	Vector ScreenLeftlegBone{};
	Vector ScreenRightThighsBone{};
	Vector ScreenRightkneesBone{};
	Vector ScreenRightlegBone{};
};
typedef struct player
{
	float dist;
	float boxMiddle;
	float h_y;
	float width;
	float height;
	float b_x;
	float b_y;
	float HeadRadius;
	float yaw;
	int entity_team;
	int health;
	int shield;
	int armorType;
	int MaxShield;
	Vector origin;
	HitBoxManager HitBox;
	bool knocked;
	bool visible;
	bool isAlive;
	char name[33];
} player; //reorder for 0 pading bytes

uint32_t check = 0xABCD;
int c = 0;
char map_name[32] = { 0 };
int aim_key = VK_XBUTTON2;
int trigger_key = VK_XBUTTON1;
bool use_nvidia = true;
bool active = true;
bool ready = false;
extern visuals v;
int aim = 0; // read
bool trigger = false;
bool strigger = false;
bool esp = false; // read
bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool aiming = false; // read
bool triggering = false;
uint64_t g_Base = 0;			 // write
float max_dist = 1500.0f * 40.0f; // read
float espDist = 400.0f;
float smooth = 150.0f;
float max_fov = 15.0f;
float rcs = 40.f;
int bone = 1;
int wp_skin_id;
int skin_id;
bool skinEnable = false;
bool thirdperson = false;
bool no_recoil = false;
bool firing_range = false;
bool control_mode = false;
bool mapradartest = false;
bool armorbaseglow = false;
bool vischeck_glow = false;
int totalSquadCount = 0;
int tmpspec = 0;
int spectators = 0; //write
int allied_spectators = 0; //write

std::vector<int> spectTeam;
std::vector<int> teamlist;

int tmpSquad;
bool cntf = true;
bool valid = false; // write
bool next = false;	// read write

uint64_t add[32];

bool k_f5 = 0;
bool k_f6 = 0;
bool k_f8 = 0;
bool k_m = 0;
bool k_f2 = 0;
bool map_opened = 0;
bool rage_mode = false;
bool IsKeyDown(int vk)
{
	return (GetAsyncKeyState(vk) & 0x8000) != 0;
}
