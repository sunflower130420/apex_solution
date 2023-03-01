#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "Game.h"
#include <thread>
#include <stdlib.h>
#include <algorithm>
#include "ip_checker.h"
Memory apex_mem;
Memory client_mem;
struct Box
{
	Vector2D topLeft;
	Vector2D bottomRight;
};

// const Vector2D crosshairPos = Vector2D(1920 / 2, 1080 / 2);
const Vector2D crosshairPos = Vector2D(1920 / 2, 1080 / 2);
// check if the crosshair  is in the box
inline bool is_point_in_box(Box &box)
{
	return (crosshairPos.x >= box.topLeft.x && crosshairPos.x <= box.bottomRight.x && crosshairPos.y >= box.topLeft.y && crosshairPos.y <= box.bottomRight.y);
}
int wp_skin_id;
int skin_id;
float rcs = 40.f;
bool firing_range = false;
bool control_mode = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f * 40.0f;
float map_dist = 2000.0f * 40.0f;
int team_player = 0;
float max_fov = 15;
const int toRead = 100;
int aim = false;
bool trigger = false;
bool strigger = false;
bool esp = false;
bool radar = false;
bool spectator_list = false;
bool item_glow = false;
bool player_glow = false;
bool isAlive = false;
bool no_recoil = false;
bool isNotShooting = false;
extern bool aim_no_recoil;
bool aiming = false;
bool triggering = false;
extern float smooth;
extern int bone;
bool thirdperson = false;
// make std vector call teamsquad
std::vector<int> teamsquad;
std::vector<int> teamsquad_tmp;
int teamsquad_size = 0;
bool actions_t = false;
bool esp_t = false;
bool radar_t = false;
bool spectators_t = false;
bool spectatorlist_t = false;
bool aim_t = false;
bool trigger_t = false;
bool strigger_t = false;
bool vars_t = false;
bool item_t = false;
bool no_recoil_t = false;
uint64_t g_Base;
uint64_t c_Base;
bool next = false;
bool valid = false;
bool lock = false;
int totalEntityCount = 0;
int totalSquadCount = 0;
bool skinEnable = false;
char map_name[32] = {0};
typedef struct player
{
	float dist = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	float HeadRadius = 0;
	float yaw = 0;
	int entity_team = 0;
	int health = 0;
	int shield = 0;
	int armorType = 0;
	int MaxShield = 0;
	Vector origin = Vector(0, 0, 0);
	HitBoxManager HitBox;
	bool knocked = false;
	bool visible = false;
	bool isAlive = false;
	char name[33] = {0};
} player;
typedef struct spectator
{
	int entity_team = 0;
	char name[33] = {0};
} spectator;
float lastvis_esp[toRead];
float lastvis_aim[toRead];
int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;

void changeSkin_wp(uint64_t LocalPlayer)
{
	if (wp_skin_id < 0)
	{
		return;
	}
	// if(isDebug)printf("changeSkin_wp\n");
	extern uint64_t g_Base;
	uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
	uint64_t wephandle = apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON);
	wephandle &= 0xffff; // localWeapon
	uint64_t wep_entity = apex_mem.Read<uint64_t>(entitylist + (wephandle << 5));
	if (wep_entity != 0 && skinEnable)
	{
		// if(isDebug)printf("trying to change weapon skin\n");
		apex_mem.Write<int>(wep_entity + OFFSET_SKIN, wp_skin_id);
	}
	if (skin_id < 0)
	{
		return;
	}
	if (LocalPlayer != 0 && skinEnable)
	{
		apex_mem.Write<int>(LocalPlayer + OFFSET_SKIN, skin_id);
	}
}
void ProcessPlayer(Entity &LPlayer, Entity &target, uint64_t entitylist, int index)
{
	int entity_team = target.getTeamId();
	int lplayer_team = LPlayer.getTeamId();
	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);
	QAngle LPlayerAngle = QAngle(LPlayer.getFPitch(), LPlayer.getFYaw(), 0);
	QAngle TargetAngle = QAngle(target.getFPitch(), target.getFYaw(), 0);
	Math::NormalizeAngles(LPlayerAngle);
	Math::NormalizeAngles(TargetAngle);
	if (dist > max_dist)
		return;
	if (!target.isAlive())
	{
		if (Math::GetFov(LPlayerAngle, TargetAngle) < 7.0f) //(LPlayer.getFYaw() - target.getFYaw() < 7.0f && LPlayer.getFPitch() - target.getFPitch() < 7.0f)
		{
			if (lplayer_team == entity_team)
				tmp_all_spec++;
			else
				tmp_spec++;
		}
		return;
	}
	if (!firing_range)
		if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
			return;
	if (aim == 2)
	{
		if ((target.lastVisTime() > lastvis_aim[index]))
		{
			float fov = CalculateFov(LPlayer, target);
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if (aimentity == target.ptr)
			{
				aimentity = tmp_aimentity = lastaimentity = 0;
			}
		}
	}
	else
	{
		float fov = CalculateFov(LPlayer, target);
		if (fov < max)
		{
			max = fov;
			tmp_aimentity = target.ptr;
		}
	}
	lastvis_aim[index] = target.lastVisTime();
}

void DoActions()
{
	actions_t = true;
	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		uint32_t counter = 0;
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
			if (LocalPlayer == 0)
				continue;

			Entity LPlayer = getEntity(LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player > 50)
			{
				continue;
			}
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}
			max = 999.0f;
			tmp_aimentity = 0;
			int entityCount = 0;
			int c = 0;
			tmp_spec = 0;
			tmp_all_spec = 0;
			apex_mem.ReadArray<char>(g_Base + OFFSET_LEVELNAME, map_name, 32);

			if (firing_range)
			{

				for (int i = 0; i < 10000; i++)
				{

					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
						continue;

					Entity Target = getEntity(centity);
					if (!Target.isDummy() && !Target.isPlayer())
					{
						continue;
					}
					c++;
					int entity_team = Target.getTeamId();
					int health = Target.getHealth();
					int shield = Target.getShield();
					if (entity_team > 0)
					{
						if (!(std::find(teamsquad_tmp.begin(), teamsquad_tmp.end(), entity_team) != teamsquad_tmp.end()))
						{
							teamsquad_tmp.push_back(entity_team);
							// clear duplicates int vector teamsquad
							std::sort(teamsquad_tmp.begin(), teamsquad_tmp.end());
							teamsquad_tmp.erase(std::unique(teamsquad_tmp.begin(), teamsquad_tmp.end()), teamsquad_tmp.end());
						}
					}

					if (player_glow && !Target.isGlowing())
					{
						float currentEntityTime = 60000.f;	 // ADDED currentEntityTime
						GlowMode mode = {101, 102, 10, 100}; // { 101,102,50,75 }; //
						GColor color;

						if ((int)Target.buffer[OFFSET_GLOW_ENABLE] != 1 || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS] != 1 || (int)Target.buffer[OFFSET_GLOW_FADE] != 872415232)
						{
							// float currentEntityTime = 60000.f;//(fhloaht)targhset->buhsffer[0xhhsEEhs4];
							if (!isnan(currentEntityTime) && currentEntityTime > 0.f)
							{

								if (Target.isBleedOut() || !Target.isAlive())
								{
									color = {27.f, 27.f, 0.f}; // Downed enemy - Yellow
								}
								else
								{
									if (shield > 100)
									{ // Heirloom armor - Red
										color = {30.f, 0.f, 0.f};
									}
									else if (shield > 75)
									{ // Purple armor - Purple
										color = {16.56, 4.14f, 18.63f};
									}
									else if (shield > 50)
									{ // Blue armor - Light blue
										color = {3.51, 10.53f, 25.65f};
									}
									else if (shield > 0)
									{ // White armor - White
										color = {27.f, 27.f, 27.f};
									}
									else if (health < 50)
									{ // Above 50% HP - Orange
										color = {33.3f, 21.5f, 0.f};
									}
									else
									{ // Below 50% HP - Green
										color = {0.f, 27.f, 0.f};
									}
								}
							}
						}

						Target.enableGlow();
						apex_mem.Write<GlowMode>(Target.ptr + GLOW_TYPE, mode); // GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel
						apex_mem.Write<GColor>(Target.ptr + GLOW_COLOR_R, color);
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}

					changeSkin_wp(LocalPlayer);
					ProcessPlayer(LPlayer, Target, entitylist, c);
					entityCount++;
					totalEntityCount = entityCount;

					// clear vector teamsquad
					teamsquad = teamsquad_tmp;
					teamsquad_size = teamsquad.size();
					teamsquad_tmp.clear();
					totalSquadCount = teamsquad_size;
					// tap_strafe(LPlayer);
				}
			}
			else
			{
				for (int i = 0; i < toRead; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
					{
						entityCount++;
						continue;
					}
					Entity Target = getEntity(centity);
					if (!Target.isPlayer())
					{
						continue;
					}
					c++;
					entityCount++;
					ProcessPlayer(LPlayer, Target, entitylist, i);
					changeSkin_wp(LocalPlayer);

					// tap_strafe(LPlayer);
					int entity_team = Target.getTeamId();
					int health = Target.getHealth();
					int shield = Target.getShield();
					if (entity_team > 0 && entity_team < 50)
					{

						// store entity_team in vector teamsquad
						if (!(std::find(teamsquad_tmp.begin(), teamsquad_tmp.end(), entity_team) != teamsquad_tmp.end()))
						{
							teamsquad_tmp.push_back(entity_team);
							// clear duplicates int vector teamsquad
							std::sort(teamsquad_tmp.begin(), teamsquad_tmp.end());
							teamsquad_tmp.erase(std::unique(teamsquad_tmp.begin(), teamsquad_tmp.end()), teamsquad_tmp.end());
						}
					}
					if (control_mode)
					{
						if (entity_team % 2)
						{
							entity_team = 1;
						}
						else
						{
							entity_team = 2;
						}
						if (team_player % 2)
						{
							team_player = 1;
						}
						else
						{
							team_player = 2;
						}
					}
					if (entity_team == team_player)
					{

						continue;
					}

					if (player_glow && !Target.isGlowing())
					{

						float currentEntityTime = 60000.f;	 // ADDED currentEntityTime
						GlowMode mode = {171, 102, 10, 100}; // { 101,102,50,75 };
						GColor color;

						if ((int)Target.buffer[OFFSET_GLOW_ENABLE] != 1 || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS] != 1 || (int)Target.buffer[OFFSET_GLOW_FADE] != 872415232)
						{
							// float currentEntityTime = 60000.f;//(fhloaht)targhset->buhsffer[0xhhsEEhs4];
							if (!isnan(currentEntityTime) && currentEntityTime > 0.f)
							{

								if (Target.isBleedOut() || !Target.isAlive())
								{
									color = {27.f, 27.f, 0.f}; // Downed enemy - Yellow
								}
								else
								{
									if (shield > 100)
									{ // Heirloom armor - Red
										color = {30.f, 0.f, 0.f};
									}
									else if (shield > 75)
									{ // Purple armor - Purple
										color = {16.56, 4.14f, 18.63f};
									}
									else if (shield > 50)
									{ // Blue armor - Light blue
										color = {3.51, 10.53f, 25.65f};
									}
									else if (shield > 0)
									{ // White armor - White
										color = {27.f, 27.f, 27.f};
									}
									else if (health < 50)
									{ // Above 50% HP - Orange
										color = {33.3f, 21.5f, 0.f};
									}
									else
									{ // Below 50% HP - Green
										color = {0.f, 27.f, 0.f};
									}
								}
							}
						}

						Target.enableGlow();
						apex_mem.Write<GlowMode>(Target.ptr + GLOW_TYPE, mode); // GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel
						apex_mem.Write<GColor>(Target.ptr + GLOW_COLOR_R, color);
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}
				}
			}

			if (!spectators && !allied_spectators)
			{
				spectators = tmp_spec;
				allied_spectators = tmp_all_spec;
			}
			else
			{
				// refresh spectators count every ~2 seconds
				counter++;
				if (counter == 10)
				{
					spectators = tmp_spec;
					allied_spectators = tmp_all_spec;
					counter = 0;
				}
			}
			if (!lock)
				aimentity = tmp_aimentity;
			else
				aimentity = lastaimentity;

			totalEntityCount = entityCount;
			// clear vector teamsquad
			teamsquad = teamsquad_tmp;
			teamsquad_size = teamsquad.size();
			teamsquad_tmp.clear();
			totalSquadCount = teamsquad_size;
		}
		while (next && g_Base != 0 && c_Base != 0 && actions_t)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	actions_t = false;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////

player players[toRead];

static void EspLoop()
{
	esp_t = true;
	while (esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
				{
					next = true;
					while (next && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Entity LPlayer = getEntity(LocalPlayer);
				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player > 50)
				{
					next = true;
					while (next && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Vector LocalPlayerPosition = LPlayer.getPosition();

				uint64_t viewRenderer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer);
				uint64_t viewMatrix = 0;
				apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrix);
				Matrix m = {};
				apex_mem.Read<Matrix>(viewMatrix, m);

				uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
				int c = 0;
				memset(players, 0, sizeof(players));
				if (firing_range)
				{

					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}

						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);

						if (!Target.isDummy() && !Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);

						if (dist > max_dist || dist < 50.0f)
						{
							continue;
						}
						// LPlayer.NullName(g_Base, i - 1);
						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePosition(8);
							// get head radius on studiohdr
							Vector Head = Target.getstudiohdr(0);
							Vector HeadHigh = Head + Vector(0, 0, 4);
							Vector Head2D, HeadHigh2D;
							WorldToScreen(Head, m.matrix, Head2D);
							WorldToScreen(HeadHigh, m.matrix, HeadHigh2D);
							float HeadRadius = Head2D.DistTo(HeadHigh2D);
							///
							WorldToScreen(HeadPosition, m.matrix, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int MaxShield = Target.getMaxShield();
							HitBoxManager HitBox = getHitbox(centity, EntityPosition, m);
							int armorType = apex_mem.Read<int>(Target.ptr + OFFSET_ARMORTYPE);

							players[c] =
								{
									dist,
									boxMiddle,
									hs.y,
									width,
									height,
									bs.x,
									bs.y,
									HeadRadius,
									Target.getFYaw(),
									entity_team,
									health,
									shield,
									armorType,
									MaxShield,
									EntityPosition,
									HitBox,
									Target.isKnocked(),
									(Target.lastVisTime() > lastvis_esp[c]),
									Target.isAlive(),
									
								};
							Target.get_name(g_Base, i - 1, &players[c].name[0]);
							lastvis_esp[c] = Target.lastVisTime();
							valid = true;
						}
					}
				}
				else
				{
					for (int i = 0; i < toRead; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}

						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);
						if (!Target.isPlayer())
						{
							continue;
						}
						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();

						if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
						{
							continue;
						}
						// std::this_thread::sleep_for(std::chrono::milliseconds(1));
						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{
							continue;
						}
						QAngle TargetAngle = QAngle(Target.getFPitch(), Target.getFYaw(), Target.getFRoll());
						QAngle LPlayerAngle = QAngle(LPlayer.getFPitch(), LPlayer.getFYaw(), LPlayer.getFRoll());
						// //printf QAngle LPlayerAngle
						//  printf("%f %f %f\n", LPlayerAngle.x, LPlayerAngle.y, LPlayerAngle.z);
						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, bs);
						// if (bs.x > 0 && bs.y > 0)
						// {
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePosition(8);
							WorldToScreen(HeadPosition, m.matrix, hs);
							// get head radius on studiohdr
							Vector Head = Target.getBonePosition(8); // Target.getstudiohdr(0);
							Vector HeadHigh = Head + Vector(0, 0, 4);
							Vector Head2D, HeadHigh2D;
							WorldToScreen(Head, m.matrix, Head2D);
							WorldToScreen(HeadHigh, m.matrix, HeadHigh2D);
							float HeadRadius = Head2D.DistTo(HeadHigh2D);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int armorType = apex_mem.Read<int>(Target.ptr + OFFSET_ARMORTYPE);
							int MaxShield = Target.getMaxShield();
							HitBoxManager HitBox = getHitbox(centity, EntityPosition, m);
							players[i] =
								{
									dist,
									boxMiddle,
									hs.y,
									width,
									height,
									bs.x,
									bs.y,
									HeadRadius,
									Target.getFYaw(),
									entity_team,
									health,
									shield,
									armorType,
									MaxShield,
									EntityPosition,
									HitBox,
									Target.isKnocked(),
									(Target.lastVisTime() > lastvis_esp[c]),
									Target.isAlive(),
								};
							Target.get_name(g_Base, i - 1, &players[i].name[0]);
							lastvis_esp[i] = Target.lastVisTime();
							valid = true;
						// }
					}
				}

				next = true;
				while (next && g_Base != 0 && c_Base != 0 && esp)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	esp_t = false;
}

static void AimbotLoop()
{
	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (aim > 0)
			{
				if (aimentity == 0 || !aiming)
				{
					lock = false;
					lastaimentity = 0;
					continue;
				}
				lock = true;
				lastaimentity = aimentity;
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
					continue;
				Entity LPlayer = getEntity(LocalPlayer);
				QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, max_fov);
				if (Angles.x == 0 && Angles.y == 0)
				{
					lock = false;
					lastaimentity = 0;
					continue;
				}
				LPlayer.SetViewAngles(Angles);
				// printf("Angles: %f %f %f\n", Angles.x, Angles.y, Angles.z);
			}
		}
	}
	aim_t = false;
}
//////////////////////////////////////////////////////////////////////////
static void TriggerBot()
{
	trigger_t = true;
	while (trigger_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			if (trigger)
			{

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (!triggering)
				{
					continue;
				}

				uint64_t LocalPlayer = apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT);
				if (LocalPlayer == 0)
					continue;
				Entity LPlayer = getEntity(LocalPlayer);
				if (aimentity == 0)
					continue;
				Entity target = getEntity(aimentity);
				uint64_t viewrender = 0;
				// isNotShooting = apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK + 0x8) != 5;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewrender);
				if (viewrender == 0)
					continue;
				uint64_t viewmatrix = apex_mem.Read<uint64_t>(viewrender + OFFSET_MATRIX);
				if (viewmatrix == 0)
					continue;
				Matrix m = {};
				apex_mem.Read<Matrix>(viewmatrix, m);
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				Vector basePos2D = Vector();
				WorldToScreen(target.getPosition(), m.matrix, basePos2D);

				Vector headPos2D = Vector();
				WorldToScreen(target.getBonePosition(8), m.matrix, headPos2D);

				float height = abs(abs(headPos2D.y) - abs(basePos2D.y));
				float width = height / 3.0f;
				// printf basePos2D.x basePos2D.y headPos2D.x headPos2D.y
				// printf("base and head pos %f %f %f %f\n", basePos2D.x, basePos2D.y, headPos2D.x, headPos2D.y);
				// check bool point_in_box is true or false

				if (basePos2D.x > 0 && basePos2D.y > 0)
				{

					Vector2D topLeft = Vector2D(basePos2D.x - width / 2, basePos2D.y - height);
					Vector2D bottomRight = Vector2D(basePos2D.x + width / 2, basePos2D.y);
					Vector2D middle = Vector2D(1920 / 2, 1080 / 2);
					Box box = {topLeft, bottomRight};

					if (topLeft.x < middle.x && topLeft.y < middle.y && bottomRight.x > middle.x && bottomRight.y > middle.y)
					{

						if (isNotShooting)
						{
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5); // mouse down
							std::this_thread::sleep_for(std::chrono::milliseconds(6));
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4); // mouse up
						}
					}
				}
			}
		}
		trigger_t = false;
	}
}

static void sTriggerbotThread()
{
	strigger_t = true;
	while (strigger_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		while (g_Base != 0 && c_Base != 0)
		{
			if (strigger)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (!triggering)
				{
					continue;
				}

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
					continue;
				Entity LPlayer = getEntity(LocalPlayer);
				if (aimentity == 0)
					continue;
				Entity target = getEntity(aimentity);
				Vector LPlayerpos = LPlayer.getPosition();

				uint64_t viewrender = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewrender);
				if (viewrender == 0)
					continue;
				uint64_t viewmatrix = apex_mem.Read<uint64_t>(viewrender + OFFSET_MATRIX);
				if (viewmatrix == 0)
					continue;
				Matrix m = {};
				apex_mem.Read<Matrix>(viewmatrix, m);
				// get skeletonpositions
				Vector Head = target.getstudiohdr(0);
				Vector Neck = target.getstudiohdr(1);
				Vector Chest = target.getstudiohdr(2);
				Vector Stomach = target.getstudiohdr(3);
				Vector Bottom = target.getstudiohdr(4);
				Vector HeadHigh = Head + Vector(0, 0, 5);
				Vector NeckHigh = Neck + Vector(0, 0, 5);
				Vector ChestHigh = Chest + Vector(0, 0, 8.5);
				Vector StomachHigh = Stomach + Vector(0, 0, 8.5);
				Vector BottomHigh = Bottom + Vector(0, 0, 8.5);
				Vector Head2D, HeadHigh2D;
				Vector Neck2D, NeckHigh2D;
				Vector Chest2D, ChestHigh2D;
				Vector Stomach2D, StomachHigh2D;
				Vector Bottom2D, BottomHigh2D;
				isNotShooting = apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK + 0x8) != 5;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (WorldToScreen(Head, m.matrix, Head2D) && WorldToScreen(HeadHigh, m.matrix, HeadHigh2D) &&
					WorldToScreen(Neck, m.matrix, Neck2D) && WorldToScreen(NeckHigh, m.matrix, NeckHigh2D) &&
					WorldToScreen(Chest, m.matrix, Chest2D) && WorldToScreen(ChestHigh, m.matrix, ChestHigh2D) &&
					WorldToScreen(Stomach, m.matrix, Stomach2D) && WorldToScreen(StomachHigh, m.matrix, StomachHigh2D) &&
					WorldToScreen(Bottom, m.matrix, Bottom2D) && WorldToScreen(BottomHigh, m.matrix, BottomHigh2D))

				{
					float HeadRadius = Head2D.y - HeadHigh2D.y;
					float NeckRadius = Neck2D.y - NeckHigh2D.y;
					float ChestRadius = Chest2D.y - ChestHigh2D.y;
					float StomachRadius = Stomach2D.y - StomachHigh2D.y;
					float BottomRadius = Bottom2D.y - BottomHigh2D.y;
					/*3440x1440*/
					// float sX = 3440 / 2;
					// float sY = 1440 / 2;
					/*1920x1080*/
					float sX = 1920 / 2;
					float sY = 1080 / 2;
					/*2560x1440*/
					// float sX = 2560 / 2;
					// float sY = 1440 / 2;

					if (((sX - Head2D.x) * (sX - Head2D.x) + (sY - Head2D.y) * (sY - Head2D.y) <= (HeadRadius * HeadRadius)) ||
						((sX - Chest2D.x) * (sX - Chest2D.x) + (sY - Chest2D.y) * (sY - Chest2D.y) <= (ChestRadius * ChestRadius)) ||
						((sX - Stomach2D.x) * (sX - Stomach2D.x) + (sY - Stomach2D.y) * (sY - Stomach2D.y) <= (StomachRadius * StomachRadius)) ||
						((sX - Bottom2D.x) * (sX - Bottom2D.x) + (sY - Bottom2D.y) * (sY - Bottom2D.y) <= (BottomRadius * BottomRadius)))
					{
						if (isNotShooting)
						{
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
							std::this_thread::sleep_for(std::chrono::milliseconds(6));
							apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
						}
					}
				}
			}
		}
		strigger_t = false;
	}
}
//////////////////////////////////////////////////////////////////////////
static void NoRecoilThread()
{
	no_recoil_t = true;
	while (no_recoil_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			if (no_recoil && !isNotShooting)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				uint64_t LocalPlayer = apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT);
				if (LocalPlayer == 0)
					continue;
				Entity LPlayer = getEntity(LocalPlayer);
				if (no_recoil)
				{
					QAngle newAngle;
					QAngle oldRecoilAngle;
					// get recoil angle
					QAngle recoilAngles = LPlayer.GetRecoil();

					// get original angles
					QAngle oldVAngles = LPlayer.GetViewAngles();

					newAngle = oldVAngles;

					// removing recoil angles from player view angles
					newAngle.x = newAngle.x + (oldRecoilAngle.x - recoilAngles.x) * (rcs / 100.f);
					newAngle.y = newAngle.y + (oldRecoilAngle.y - recoilAngles.y) * (rcs / 100.f);

					// setting viewangles to new angles

					LPlayer.SetViewAngles(newAngle);
					// setting old recoil angles to current recoil angles
					oldRecoilAngle = recoilAngles;
					// normalize view angles
					Math::NormalizeAngles(oldRecoilAngle);
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	no_recoil_t = false;
}
static void set_vars(uint64_t add_addr)
{
	printf("Reading client vars...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	// Get addresses of client vars
	uint64_t check_addr = 0;
	client_mem.Read<uint64_t>(add_addr, check_addr);
	uint64_t aim_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t), aim_addr);
	uint64_t esp_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 2, esp_addr);
	uint64_t aiming_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 3, aiming_addr);
	uint64_t g_Base_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 4, g_Base_addr);
	uint64_t next_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 5, next_addr);
	uint64_t player_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 6, player_addr);
	uint64_t valid_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 7, valid_addr);
	uint64_t max_dist_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 8, max_dist_addr);
	uint64_t item_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 9, item_glow_addr);
	uint64_t player_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 10, player_glow_addr);
	uint64_t aim_no_recoil_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 11, aim_no_recoil_addr);
	uint64_t smooth_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 12, smooth_addr);
	uint64_t max_fov_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 13, max_fov_addr);
	uint64_t bone_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 14, bone_addr);
	uint64_t trigger_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 15, trigger_addr);
	uint64_t triggering_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 16, triggering_addr);
	uint64_t rcs_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 17, rcs_addr);
	uint64_t no_recoil_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 18, no_recoil_addr);
	uint64_t firing_range_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 19, firing_range_addr);
	uint64_t weapon_skin_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 20, weapon_skin_addr);
	uint64_t strigger_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 21, strigger_addr);
	uint64_t c_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 22, c_addr);
	uint64_t skin_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 23, skin_addr);
	uint64_t skinEnable_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 24, skinEnable_addr);
	uint64_t control_mode_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 25, control_mode_addr);
	uint64_t totalSquadCount_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 26, totalSquadCount_addr);
	uint64_t spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 27, spectators_addr);
	uint64_t allied_spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 28, allied_spectators_addr);
	printf("%p\n", c_addr);
	uint32_t check = 0;
	client_mem.Read<uint32_t>(check_addr, check);
	if (check != 0xAABBCC)
	{
		printf("Incorrect values read. Check if the add_off is correct. Quitting.\n");
		active = false;
		return;
	}
	vars_t = true;
	while (vars_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (c_Base != 0 && g_Base != 0)
		{
			client_mem.Write<uint32_t>(check_addr, 0);
			printf("\nReady\n");
		}
		while (c_Base != 0 && g_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			client_mem.Write<uint64_t>(g_Base_addr, g_Base);
			client_mem.Write<int>(spectators_addr, spectators);
			client_mem.Write<int>(allied_spectators_addr, allied_spectators);
			client_mem.Read<int>(aim_addr, aim);
			client_mem.Read<bool>(esp_addr, esp);
			client_mem.Read<bool>(aiming_addr, aiming);
			client_mem.Read<float>(max_dist_addr, max_dist);
			client_mem.Read<bool>(item_glow_addr, item_glow);
			client_mem.Read<bool>(player_glow_addr, player_glow);
			client_mem.Read<bool>(aim_no_recoil_addr, aim_no_recoil);
			client_mem.Read<float>(smooth_addr, smooth);
			client_mem.Read<float>(max_fov_addr, max_fov);
			client_mem.Read<int>(bone_addr, bone);
			client_mem.Read<bool>(trigger_addr, trigger);
			client_mem.Read<bool>(triggering_addr, triggering);
			client_mem.Read<float>(rcs_addr, rcs);
			client_mem.Read<bool>(no_recoil_addr, no_recoil);
			client_mem.Read<bool>(firing_range_addr, firing_range);
			client_mem.Read<int>(weapon_skin_addr, wp_skin_id);
			client_mem.Read<bool>(strigger_addr, strigger);
			client_mem.Write<int>(c_addr, totalEntityCount);
			client_mem.Read<int>(skin_addr, skin_id);
			client_mem.Read<bool>(skinEnable_addr, skinEnable);
			client_mem.Read<bool>(control_mode_addr, control_mode);
			client_mem.Write<int>(totalSquadCount_addr, totalSquadCount);
			if (esp && next)
			{
				if (valid)
					client_mem.WriteArray<player>(player_addr, players, toRead);

				client_mem.Write<bool>(valid_addr, valid);
				client_mem.Write<bool>(next_addr, true); // next
				bool next_val = false;
				do
				{
					client_mem.Read<bool>(next_addr, next_val);
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				} while (next_val && g_Base != 0 && c_Base != 0);
				next = false;
			}
		}
	}
	vars_t = false;
}

static void item_glow_t()
{
	item_t = true;
	while (item_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		int k = 0;
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			uint64_t entitylist = g_Base + offsets::OFFSET_ENTITYLIST;
			if (item_glow)
			{
				for (int i = 0; i < 15000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					Item item = getItem(centity);

					if (item.isBox())
					{
						apex_mem.Write<int>(centity + 0x262, 16256);
						apex_mem.Write<int>(centity + 0x2dc, 1193322764);
						apex_mem.Write<int>(centity + 0x3c8, 7);
						apex_mem.Write<int>(centity + 0x3d0, 2);
					}

					if (item.isTrap())
					{
						apex_mem.Write<int>(centity + 0x262, 16256);
						apex_mem.Write<int>(centity + 0x2dc, 1193322764);
						apex_mem.Write<int>(centity + 0x3c8, 7);
						apex_mem.Write<int>(centity + 0x3d0, 2);
					}

					if (item.isItem() && !item.isGlowing())
					{
						item.enableGlow();
					}
				}
				k = 1;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				if (k == 1)
				{
					for (int i = 0; i < 15000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
							continue;

						Item item = getItem(centity);

						if (item.isItem() && item.isGlowing())
						{
							item.disableGlow();
						}
					}
					k = 0;
				}
			}
		}
	}
	item_t = false;
}

int main(int argc, char *argv[])
{
	if (geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}
	// std::vector<std::string> addresses = getlocal_IP_Address();
	// for (const auto& address : addresses) {
	//     std::cout << "Local IP address: " << address << std::endl;
	// }
	const char *cl_proc = "solution_app.exe";
	const char *ap_proc = "R5Apex.exe";
	// Client "add" offset
	uint64_t add_off = 0x3edb00;
	// check offset is loaded
	if (offset_manager::LoadOffsets() == false)
	{
		printf("Error: Failed to load offsets\n");
		return 0;
		exit(0);
	}
	std::thread aimbot_thr;
	std::thread esp_thr;
	std::thread actions_thr;
	std::thread itemglow_thr;
	std::thread vars_thr;
	std::thread trigger_thr;
	std::thread strigger_thr;
	std::thread rcs_thr;
	while (active)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (aim_t)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				item_t = false;
				trigger_t = false;
				strigger_t = false;
				no_recoil_t = false;
				g_Base = 0;
				aimbot_thr.~thread();
				esp_thr.~thread();
				actions_thr.~thread();
				itemglow_thr.~thread();
				trigger_thr.~thread();
				strigger_thr.~thread();
				rcs_thr.~thread();
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");
			apex_mem.open_proc(ap_proc);
			if (apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("\nApex process found\n");
				printf("Base: %lx\n", g_Base);
				aimbot_thr = std::thread(AimbotLoop);
				esp_thr = std::thread(EspLoop);
				actions_thr = std::thread(DoActions);
				itemglow_thr = std::thread(item_glow_t);
				trigger_thr = std::thread(TriggerBot);
				strigger_thr = std::thread(sTriggerbotThread);
				rcs_thr = std::thread(NoRecoilThread);
				aimbot_thr.detach();
				trigger_thr.detach();
				strigger_thr.detach();
				esp_thr.detach();
				// spectators_thr.detach();
				actions_thr.detach();
				itemglow_thr.detach();
				rcs_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}

		if (client_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (vars_t)
			{
				vars_t = false;
				c_Base = 0;

				vars_thr.~thread();
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for client process...\n");
			client_mem.open_proc(cl_proc);
			if (client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nClient process found\n");
				printf("Base: %lx\n", c_Base);

				vars_thr = std::thread(set_vars, c_Base + add_off);
				vars_thr.detach();
			}
		}
		else
		{
			client_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return 0;
}