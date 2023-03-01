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
	ImColor(0, 0, 0),         // ID  0:	DOES NOT EXIST
	ImColor(0, 0, 0),         // ID  1:	Spectators
	ImColor(17, 124, 141),    // ID  2:	Tealish			( Team 1  )
	ImColor(33, 71, 103),     // ID  3:	Dark Blue		( Team 2  )
	ImColor(31, 84, 205),     // ID  4:	Blue			( Team 3  )
	ImColor(68, 42, 96),      // ID  5:	Light Purple	( Team 4  )
	ImColor(110, 44, 111),    // ID  6:	Light Green		( Team 5  )
	ImColor(173, 45, 119),    // ID  7:	Light Magenta	( Team 6  )
	ImColor(175, 30, 81),     // ID  8:	Green			( Team 7  )
	ImColor(194, 5, 15),      // ID  9:	Red				( Team 8  )
	ImColor(197, 67, 32),     // ID 10:	Pink			( Team 9  )
	ImColor(119, 32, 22),     // ID 11:	Brownish Orange ( Team 10 )
	ImColor(158, 60, 16),     // ID 12:	Orange			( Team 11 )
	ImColor(118, 75, 5),      // ID 13:	Olive Green		( Team 12 )
	ImColor(170, 101, 34),    // ID 14:	Yellowish Orange( Team 13 )
	ImColor(150, 125, 0),     // ID 15:	Yellow-Green	( Team 14 )
	ImColor(133, 147, 10),    // ID 16:	Light Green		( Team 15 )
	ImColor(73, 88, 3),       // ID 17:	Dark Green		( Team 16 )
	ImColor(107, 143, 66),    // ID 18:	Light Olive		( Team 17 )
	ImColor(59, 131, 57),     // ID 19:	Olive			( Team 18 )
	ImColor(47, 90, 26),      // ID 20:	Brown			( Team 19 )
	ImColor(5, 116, 88),      // ID 21:	Purple			( Team 20 )
	ImColor(110, 50, 6),      // ID 22:	Bluish Green	( Team 21 )
	ImColor(14, 88, 106),     // ID 23:	Sky Blue		( Team 22 )
	ImColor(10, 130, 148),    // ID 24:	Bright Green	( Team 23 )
	ImColor(27, 71, 105),     // ID 25:	Pale Blue		( Team 24 )
	ImColor(38, 84, 195),     // ID 26:	Bright Blue		( Team 25 )
	ImColor(70, 48, 96),      // ID 27:	Purple			( Team 26 )
	ImColor(110, 44, 111),    // ID 28:	Light Green		( Team 27 )
	ImColor(173, 45, 119),    // ID 29:	Light Magenta	( Team 28 )
	ImColor(176, 28, 81),     // ID 30:	Light Red		( Team 29 )
	ImColor(195, 0, 11)       // ID 31:	Bright Red		( Team 30 )
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
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	int armorType = 0;
	int MaxShield = 0;
	HitBoxManager HitBox;
	bool isAlive = false;
	float HeadRadius = 0;
	float yaw = 0;
	Vector origin;
	char name[33] = { 0 };

} player;

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
bool aim_no_recoil = false;
bool aiming = false; // read
bool triggering = false;
uint64_t g_Base = 0;			 // write
float max_dist = 1500.0f * 40.0f; // read
float espDist = 150.0f;
float smooth = 130.0f;
float max_fov = 3.0f;
float rcs = 60.f;
int bone = 2;
bool bestbone_bool = false;
int wp_skin_id;
int skin_id;
bool skinEnable = false;
bool thirdperson = false;
bool no_recoil = false;
bool firing_range = false;
bool control_mode = false;
bool mapradartest = false;
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

player players[100];
std::vector<player> tmp_spectatingMe;
std::vector<player> spectatingMe;