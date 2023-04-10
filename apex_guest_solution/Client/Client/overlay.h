#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <Dwmapi.h> 
#pragma comment(lib, "dwmapi.lib")
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <cwchar>
#include <thread>
#include <string>
#include "XorString.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d11.h>
#include "vector.h"
#include "custom.h"
#include "icon.h"
#include <fstream>
#include <iostream>
#include "json/single_include/nlohmann/json.hpp"
using json = nlohmann::json;
#pragma comment(lib, "d3d11.lib")

#define GREEN ImColor(0, 255, 0)
#define RED ImColor(255, 0, 0)
#define BLUE ImColor(0, 0, 255)
#define ORANGE ImColor(255, 165, 0)
#define WHITE ImColor(255, 255, 255)
#define BLACK ImColor(0, 0, 0)
extern char map_name[32];
typedef struct visuals
{
	bool box = false;
	bool cornerbox = false;
	bool line = false;
	bool distance = false;
	bool healthbar = false;
	bool shieldbar = false;
	bool seerbar = false;
	bool name = true;
	bool skeleton = false;
	bool simplespec = true;
	bool spectator = false;
	float SkeltonPx = 1.0f;
	bool FOV = false;
	bool spectator_id = false;
	bool radar_map = true;
	int radar_shadow_alpha = 100;
	int radar_shadow_color = 75;
	int radar_shadow_size = 3;
	int radar_arrow_size = 12;
	bool radar_show_teamId = true;
	int radar_teamId_y = 20;
	bool radar_arrow_shadow = true;
	int tab = 1;

}visuals;

class UI
{
public:
	void Start();
	DWORD CreateOverlay();
	void Clear();
	int getWidth();
	int getHeight();
	void RenderMenu();
	void RenderSpec();
	void RenderEsp();
	void ClickThrough(bool v);
	void WindowInfo();
	void DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width);
	void DrawBox(ImColor color, float x, float y, float w, float h);
	void DrawCircleFOV(ImVec2 center, float radius, ImColor col,float thick);
	void DrawCornerBox( ImVec2 rect,  ImVec2 size, ImColor color, float width);
	void Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect);
	void StringOutlined(ImVec2 pos, ImColor color, const char* text, bool centerText);
	void DrawOutlinedText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect, bool centerText);
	void RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags);
	void ProgressBar(float x, float y, float w, float h, int value, int v_max);
	void String(ImVec2 pos, ImColor color, const char* text);
	void DrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color);
	void DrawHexagon(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col, float thickness);
	void DrawHexagonFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col);
	void DrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health, float size);

	void DrawFilledArrow(float x, float y, float size, ImColor color, float yaw);

	void drawPlayerOnFullRadar(float x, float y, ImColor color, float yaw, int teamId);
	
private:
	bool running;
	HWND overlayHWND;
};
namespace setting
{
	void Save(json& j);
	void Load(json& j);
}
