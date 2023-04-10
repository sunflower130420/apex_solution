#include "main.h"

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
void UI::RenderEsp()
{
	next = false;
	if (g_Base != 0 && esp)
	{
		memset(players, 0, sizeof(players));
		while (!next && esp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		if (next && valid)
		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)getWidth(), (float)getHeight()));
			ImGui::Begin(XorStr("##esp"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus);

			for (int i = 0; i < 100; i++)
			{

				if (players[i].health > 0)
				{
					std::string distance = std::to_string(players[i].dist / 39.62);
					distance = distance.substr(0, distance.find('.')) + "m(" + std::to_string(players[i].entity_team) + ")";
					int dist = (int)players[i].dist / 39.62;
					if (map_opened) {
						ImColor color = teamColor[(players[i].entity_team)];
						drawPlayerOnFullRadar(players[i].origin.x, players[i].origin.y, color, players[i].yaw, players[i].entity_team);
					}
					else if (dist < espDist)
					{
						if (v.box)
						{
							if (players[i].visible)
							{
								if (players[i].dist < 1600.0f)
									DrawBox(RED, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); // BOX
								else
									DrawBox(ORANGE, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); // BOX
							}
							else
							{
								DrawBox(WHITE, players[i].boxMiddle, players[i].h_y, players[i].width, players[i].height); // white if player not visible
							}
						}
						if (v.cornerbox)
						{
							if (players[i].visible)
							{
								if (players[i].dist < 1600.0f)
									DrawCornerBox(ImVec2(players[i].boxMiddle, players[i].h_y), ImVec2(players[i].width, players[i].height), RED, 1.0f); // CORNERBOX
								else
									DrawCornerBox(ImVec2(players[i].boxMiddle, players[i].h_y), ImVec2(players[i].width, players[i].height), ORANGE, 1.0f);
							}
							else
							{
								DrawCornerBox(ImVec2(players[i].boxMiddle, players[i].h_y), ImVec2(players[i].width, players[i].height), WHITE, 1.0f);
							}
						}
						if (v.skeleton)
						{

							HitBoxManager HitBox = players[i].HitBox;
							ImColor skCol;
							if (players[i].visible)
							{
								ImColor skCol = GREEN;
								DrawCircleFOV(ImVec2(HitBox.ScreenHeadBone.x, HitBox.ScreenHeadBone.y), players[i].HeadRadius, skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenHeadBone.x, HitBox.ScreenHeadBone.y), ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), ImVec2(HitBox.ScreenChestBone.x, HitBox.ScreenChestBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenChestBone.x, HitBox.ScreenChestBone.y), ImVec2(HitBox.ScreenWaistBone.x, HitBox.ScreenWaistBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenWaistBone.x, HitBox.ScreenWaistBone.y), ImVec2(HitBox.ScreenBotmBone.x, HitBox.ScreenBotmBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), ImVec2(HitBox.ScreenLeftshoulderBone.x, HitBox.ScreenLeftshoulderBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftshoulderBone.x, HitBox.ScreenLeftshoulderBone.y), ImVec2(HitBox.ScreenLeftelbowBone.x, HitBox.ScreenLeftelbowBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftelbowBone.x, HitBox.ScreenLeftelbowBone.y), ImVec2(HitBox.ScreenLeftHandBone.x, HitBox.ScreenLeftHandBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), ImVec2(HitBox.ScreenRightshoulderBone.x, HitBox.ScreenRightshoulderBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightshoulderBone.x, HitBox.ScreenRightshoulderBone.y), ImVec2(HitBox.ScreenRightelbowBone.x, HitBox.ScreenRightelbowBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightelbowBone.x, HitBox.ScreenRightelbowBone.y), ImVec2(HitBox.ScreenRightHandBone.x, HitBox.ScreenRightHandBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenBotmBone.x, HitBox.ScreenBotmBone.y), ImVec2(HitBox.ScreenLeftThighsBone.x, HitBox.ScreenLeftThighsBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftThighsBone.x, HitBox.ScreenLeftThighsBone.y), ImVec2(HitBox.ScreenLeftkneesBone.x, HitBox.ScreenLeftkneesBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftkneesBone.x, HitBox.ScreenLeftkneesBone.y), ImVec2(HitBox.ScreenLeftlegBone.x, HitBox.ScreenLeftlegBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenBotmBone.x, HitBox.ScreenBotmBone.y), ImVec2(HitBox.ScreenRightThighsBone.x, HitBox.ScreenRightThighsBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightThighsBone.x, HitBox.ScreenRightThighsBone.y), ImVec2(HitBox.ScreenRightkneesBone.x, HitBox.ScreenRightkneesBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightkneesBone.x, HitBox.ScreenRightkneesBone.y), ImVec2(HitBox.ScreenRightlegBone.x, HitBox.ScreenRightlegBone.y), skCol, v.SkeltonPx);
							}
							else
							{
								ImColor skCol = RED;
								DrawCircleFOV(ImVec2(HitBox.ScreenHeadBone.x, HitBox.ScreenHeadBone.y), players[i].HeadRadius, skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenHeadBone.x, HitBox.ScreenHeadBone.y), ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), ImVec2(HitBox.ScreenChestBone.x, HitBox.ScreenChestBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenChestBone.x, HitBox.ScreenChestBone.y), ImVec2(HitBox.ScreenWaistBone.x, HitBox.ScreenWaistBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenWaistBone.x, HitBox.ScreenWaistBone.y), ImVec2(HitBox.ScreenBotmBone.x, HitBox.ScreenBotmBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), ImVec2(HitBox.ScreenLeftshoulderBone.x, HitBox.ScreenLeftshoulderBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftshoulderBone.x, HitBox.ScreenLeftshoulderBone.y), ImVec2(HitBox.ScreenLeftelbowBone.x, HitBox.ScreenLeftelbowBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftelbowBone.x, HitBox.ScreenLeftelbowBone.y), ImVec2(HitBox.ScreenLeftHandBone.x, HitBox.ScreenLeftHandBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenNeckBone.x, HitBox.ScreenNeckBone.y), ImVec2(HitBox.ScreenRightshoulderBone.x, HitBox.ScreenRightshoulderBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightshoulderBone.x, HitBox.ScreenRightshoulderBone.y), ImVec2(HitBox.ScreenRightelbowBone.x, HitBox.ScreenRightelbowBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightelbowBone.x, HitBox.ScreenRightelbowBone.y), ImVec2(HitBox.ScreenRightHandBone.x, HitBox.ScreenRightHandBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenBotmBone.x, HitBox.ScreenBotmBone.y), ImVec2(HitBox.ScreenLeftThighsBone.x, HitBox.ScreenLeftThighsBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftThighsBone.x, HitBox.ScreenLeftThighsBone.y), ImVec2(HitBox.ScreenLeftkneesBone.x, HitBox.ScreenLeftkneesBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenLeftkneesBone.x, HitBox.ScreenLeftkneesBone.y), ImVec2(HitBox.ScreenLeftlegBone.x, HitBox.ScreenLeftlegBone.y), skCol, v.SkeltonPx);

								DrawLine(ImVec2(HitBox.ScreenBotmBone.x, HitBox.ScreenBotmBone.y), ImVec2(HitBox.ScreenRightThighsBone.x, HitBox.ScreenRightThighsBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightThighsBone.x, HitBox.ScreenRightThighsBone.y), ImVec2(HitBox.ScreenRightkneesBone.x, HitBox.ScreenRightkneesBone.y), skCol, v.SkeltonPx);
								DrawLine(ImVec2(HitBox.ScreenRightkneesBone.x, HitBox.ScreenRightkneesBone.y), ImVec2(HitBox.ScreenRightlegBone.x, HitBox.ScreenRightlegBone.y), skCol, v.SkeltonPx);
							}
						}

						if (v.line)
						{
							if (players[i].dist < 1000.0f)
							{
								if (players[i].visible)
								{
									DrawLine(ImVec2((float)(getWidth() / 2), (float)getHeight() / 2), ImVec2(players[i].HitBox.ScreenChestBone.x, players[i].HitBox.ScreenChestBone.y), GREEN, 1.5f); // LINE FROM MIDDLE SCREEN

								}
							}
						}
						if (v.distance)
						{
							if (players[i].visible)
								String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), GREEN, distance.c_str()); // DISTANCE
							else
								String(ImVec2(players[i].boxMiddle, (players[i].b_y + 1)), RED, distance.c_str()); // DISTANCE
						}
						if (players[i].dist < 3000.0f)
						{
							if (v.seerbar)
							{
								DrawSeerLikeHealth(players[i].b_x, (players[i].b_y + players[i].height + 10), players[i].shield, players[i].MaxShield, players[i].armorType, players[i].health, 2); // Seer health esp
							}
							if (v.healthbar)
								ProgressBar((players[i].b_x - (players[i].width / 2.0f) - 4), (players[i].b_y - players[i].height), 3, players[i].height, players[i].health, 100); // health bar
							if (v.shieldbar)
								ProgressBar((players[i].b_x + (players[i].width / 2.0f) + 1), (players[i].b_y - players[i].height), 3, players[i].height, players[i].shield, 125); // shield bar
						}
						if (v.FOV)
						{
							DrawCircleFOV(ImVec2((float)(getWidth() / 2), (float)getHeight() / 2), max_fov * 8, WHITE, 1.0f);
						}
						if (v.name)
						{
							std::string name = "[" + std::to_string(players[i].entity_team) + "]" + players[i].name;
							ImColor color = teamColor[(players[i].entity_team)];
							StringOutlined(ImVec2(players[i].b_x, (players[i].b_y - players[i].height - 17)), color, name.c_str(), true);
						}
					}
				}
			}

		}
		ImGui::End();
	}
}

void UI::WindowInfo()
{
	ImGui::SetNextWindowPos(ImVec2(1640, 10));
	ImGui::Begin(XorStr("##info"), (bool*)true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize); //| ImGuiWindowFlags_NoBackground
	ImGui::SetWindowFontScale(1.0f);
	std::string ch;
	if (v.simplespec)
	{
		ImGui::TextColored(RED, "Team:%d", totalSquadCount); ImGui::SameLine(); ImGui::Text("-"); ImGui::SameLine(); ImGui::TextColored(GREEN, "Ent:%d", c);
		//ImGui::SameLine(); ImGui::TextColored(RED, "Rage:%d", rage_mode);
		ImGui::TextColored(RED, "Spec:%d", spectators); ImGui::SameLine(); ImGui::Text("-"); ImGui::SameLine(); ImGui::TextColored(GREEN, "Teamspec:%d", allied_spectators);
	}
	ImGui::End();
}
uint64_t add[32];
int main(int argc, char** argv)
{
	
	add[0]   =  (uintptr_t)&check;
	add[1]   =  (uintptr_t)&aim;
	add[2]   =  (uintptr_t)&esp;
	add[3]   =  (uintptr_t)&aiming;
	add[4]   =  (uintptr_t)&g_Base;
	add[5]   =  (uintptr_t)&next;
	add[6]   =  (uintptr_t)&players[0];
	add[7]   =  (uintptr_t)&valid;
	add[8]   =  (uintptr_t)&max_dist;
	add[9]   =  (uintptr_t)&item_glow;
	add[10]  =  (uintptr_t)&player_glow;
	add[11]  =  (uintptr_t)&aim_no_recoil;
	add[12]  =  (uintptr_t)&smooth;
	add[13]  =  (uintptr_t)&max_fov;
	add[14]  =  (uintptr_t)&bone;
	add[15]  =  (uintptr_t)&trigger;
	add[16]  =  (uintptr_t)&triggering;
	add[17]  =  (uintptr_t)&rcs;
	add[18]  =  (uintptr_t)&no_recoil;
	add[19]  =  (uintptr_t)&firing_range;
	add[20]  =  (uintptr_t)&wp_skin_id;
	add[21]  =  (uintptr_t)&strigger;
	add[22]  =  (uintptr_t)&c;
	add[23]  =  (uintptr_t)&skin_id;
	add[24]  =  (uintptr_t)&skinEnable;
	add[25]  =  (uintptr_t)&control_mode;
	add[26]  =  (uintptr_t)&totalSquadCount;
	add[27]  =  (uintptr_t)&spectators;
	add[28]  =  (uintptr_t)&allied_spectators;
	add[29]  =  (uintptr_t)&map_name;
	add[30]  =  (uintptr_t)&vischeck_glow;
	add[31]  =  (uintptr_t)&armorbaseglow;
	printf(XorStr("add offset: 0x%I64x\n"), (uint64_t)&add[0] - (uint64_t)GetModuleHandle(NULL));
	UI ov1 = UI();
	ov1.Start();
	printf(XorStr("Waiting for host process...\n"));
	while (check == 0xABCD)
	{
		if (IsKeyDown(VK_F4))
		{
			active = false;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	if (active)
	{
		ready = true;
		printf(XorStr("Ready\n"));
	}

	while (active)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (IsKeyDown(VK_F4))
		{
			active = false;
		}

		if (IsKeyDown(VK_F5) && k_f5 == 0)
		{
			k_f5 = 1;
			esp = !esp;
		}
		else if (!IsKeyDown(VK_F5) && k_f5 == 1)
		{
			k_f5 = 0;
		}

		if (IsKeyDown(VK_F6) && k_f6 == 0)
		{
			k_f6 = 1;
			switch (aim)
			{
			case 0:
				aim = 1;
				break;
			case 1:
				aim = 2;
				break;
			case 2:
				aim = 0;
				break;
			default:
				break;
			}
		}
		else if (!IsKeyDown(VK_F6) && k_f6 == 1)
		{
			k_f6 = 0;
		}

		if (IsKeyDown(VK_F8) && k_f8 == 0)
		{
			k_f8 = 1;

		}
		else if (!IsKeyDown(VK_F8) && k_f8 == 1)
		{
			k_f8 = 0;

		}
		if (IsKeyDown(VK_END))
		{

			mapradartest = true;
			Sleep(300);
			mapradartest = false;


		}
		if (IsKeyDown(VK_LEFT))
		{
			if (max_dist > 100.0f * 40.0f)
				max_dist -= 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(VK_RIGHT))
		{
			if (max_dist < 800.0f * 40.0f)
				max_dist += 50.0f * 40.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(130));
		}

		if (IsKeyDown(aim_key))
		{
			aiming = true;
		}

		else if (!IsKeyDown(aim_key))
		{
			aiming = false;
		}
		if (IsKeyDown(trigger_key))
		{
			triggering = true;
		}
		else if (!IsKeyDown(trigger_key))
		{
			triggering = false;
		}
		if (IsKeyDown(0x4D) && k_m == 0) {// m_key
			if (strncmp(map_name, "mp_rr_canyonlands_staging", 25) != 0) { //not Firing range
				map_opened = !map_opened;
				k_m = 1;
			}
		}
		else if (!IsKeyDown(0x4D) && k_m == 1)
		{
			k_m = 0;
		}
	}
	ready = false;
	ov1.Clear();
	return 0;
}