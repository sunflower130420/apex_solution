#include "overlay.h"
// boolean flags
extern bool esp;
extern bool skinEnable;
extern bool item_glow;
extern bool player_glow;
extern bool aim_no_recoil;
extern bool ready;
extern bool use_nvidia;
extern bool trigger;
extern bool no_recoil;
extern bool firing_range;
extern bool control_mode;
extern bool strigger;

// integer values
extern int aim;
extern int wp_skin_id;
extern int skin_id;
extern int bone;

// floating-point values
extern float max_dist;
extern float smooth;
extern float max_fov;
extern float rcs;
extern float espDist;

int width;
int height;
bool k_leftclick = false;
bool k_ins = false;
bool show_menu = false;
visuals v;
extern bool IsKeyDown(int vk);

LONG nv_default = WS_POPUP | WS_CLIPSIBLINGS;
LONG nv_default_in_game = nv_default | WS_DISABLED;
LONG nv_edit = nv_default_in_game | WS_VISIBLE;

LONG nv_ex_default = WS_EX_TOOLWINDOW;
LONG nv_ex_edit = nv_ex_default | WS_EX_LAYERED | WS_EX_TRANSPARENT;
LONG nv_ex_edit_menu = nv_ex_default | WS_EX_TRANSPARENT;
ImVec2 rotate(ImVec2 point, float angle) {
	float radAngle = angle * (M_PI / 180);
	float x = (float)(cos(radAngle) * point.x - sin(radAngle) * point.y);
	float y = (float)(sin(radAngle) * point.x + cos(radAngle) * point.y);
	return ImVec2(x, y);
}
static DWORD WINAPI StaticMessageStart(void* Param)
{
	UI* ov = (UI*)Param;
	ov->CreateOverlay();
	return 0;
}

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
	wchar_t className[255] = L"";
	GetClassName(hwnd, className, 255);
	if (use_nvidia)
	{
		if (wcscmp(XorStrW(L"CEF-OSC-WIDGET"), className) == 0) //Nvidia overlay
		{
			HWND* w = (HWND*)lParam;
			if (GetWindowLong(hwnd, GWL_STYLE) != nv_default && GetWindowLong(hwnd, GWL_STYLE) != nv_default_in_game)
				return TRUE;
			*w = hwnd;
			return TRUE;
		}
	}
	return TRUE;
}
// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void UI::RenderMenu()
{
	static bool aim_enable = false;
	static bool vis_check = false;
	if (aim > 0)
	{
		aim_enable = true;
		if (aim > 1)
		{
			vis_check = true;
		}
		else
		{
			vis_check = false;
		}
	}
	else
	{
		aim_enable = false;
		vis_check = false;
	}
	ImGui::SetNextWindowSize(ImVec2(520.000f, 280.000f), ImGuiCond_Once);
	///////////////////////////
	ImGui::Begin(XorStr("Apex"), NULL, 58);
	if (ImGui::Button(XorStr("Main"), ImVec2(100, 0)))
		v.tab = 1;
	ImGui::SameLine();
	if (ImGui::Button(XorStr("Visuals"), ImVec2(100, 0)))
		v.tab = 2;
	ImGui::SameLine();
	if (ImGui::Button(XorStr("Config"), ImVec2(100, 0)))
		v.tab = 3;
	ImGui::Separator();
	///////////////////////////
	switch (v.tab)
	{
	case 1: // Main
		ImGui::Text(XorStr("Main options:"));
		ImGui::Checkbox(XorStr("ESP"), &esp);
		ImGui::Checkbox(XorStr("AIM"), &aim_enable);
		if (aim_enable)
		{
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("Visibility check"), &vis_check);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("aimbot recoil"), &aim_no_recoil);
			ImGui::SameLine();
			ImGui::Checkbox(XorStr("rcsNoRecoil"), &no_recoil);
			if (vis_check)
			{
				aim = 2;
			}
			else
			{
				aim = 1;
			}
		}
		else
		{
			aim = 0;
		}
		ImGui::Checkbox(XorStr("Glow items"), &item_glow);
		ImGui::Checkbox(XorStr("Glow players"), &player_glow);
		ImGui::Checkbox(XorStr("Triggerbot"), &strigger);
		ImGui::Checkbox(XorStr("Firing range"), &firing_range);
		ImGui::SameLine();
		ImGui::Checkbox(XorStr("Control Mode"), &control_mode);
		break;
	case 2: // Visuals
		ImGui::Text(XorStr("ESP options:"));
		ImGui::Checkbox(XorStr("Box"), &v.box);
		ImGui::SameLine(0, 70.0f);
		ImGui::Checkbox(XorStr("CornerBox"), &v.cornerbox);
		ImGui::Checkbox(XorStr("Name"), &v.name);
		ImGui::Checkbox(XorStr("Line"), &v.line);
		ImGui::Checkbox(XorStr("Skeleton"), &v.skeleton);
		ImGui::Checkbox(XorStr("Distance"), &v.distance);
		ImGui::Checkbox(XorStr("Seer health bar"), &v.seerbar);
		ImGui::Checkbox(XorStr("Spectator/Team count"), &v.simplespec);
		ImGui::Checkbox(XorStr("Health bar"), &v.healthbar);
		ImGui::SameLine(0, 70.0f);
		ImGui::Checkbox(XorStr("Shield bar"), &v.shieldbar);
		break;
	case 3: //Configs
		ImGui::Text(XorStr("Max distance:"));
		ImGui::SliderFloat(XorStr("##1"), &espDist, 100.0f, 15000.0f, XorStr("%.2f"));
		ImGui::SameLine();
		ImGui::Text(XorStr("(%d meters)"), (int)(espDist));
		ImGui::Text(XorStr("Smooth aim value:"));
		ImGui::SliderFloat(XorStr("##2"), &smooth, 12.0f, 150.0f, XorStr("%.2f"));
		ImGui::Text(XorStr("Max FOV:"));
		ImGui::SameLine(0, 70.0f);
		ImGui::Checkbox(XorStr("FOV"), &v.FOV);
		ImGui::SliderFloat(XorStr("##3"), &max_fov, 5.0f, 25.0f, XorStr("%.2f"));
		ImGui::Text(XorStr("Aim at (bone id):"));
		ImGui::SliderInt(XorStr("##4"), &bone, 0, 8);
		ImGui::Text(XorStr("rcsRecoil:"));
		ImGui::SliderFloat(XorStr("##5"), &rcs, 0, 100);
		break;
	}
	//ImGui::Columns(1);
	ImGui::Text(XorStr("Overlay FPS: %.3f ms/frame (%.1f FPS)"), 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}
void UI::ClickThrough(bool v)
{
	if (v)
	{
		nv_edit = nv_default_in_game | WS_VISIBLE;
		if (GetWindowLong(overlayHWND, GWL_EXSTYLE) != nv_ex_edit)
			SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_edit);
	}
	else
	{
		nv_edit = nv_default | WS_VISIBLE;
		if (GetWindowLong(overlayHWND, GWL_EXSTYLE) != nv_ex_edit_menu)
			SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_edit_menu);
	}
}


DWORD UI::CreateOverlay()
{
	EnumWindows(EnumWindowsCallback, (LPARAM)&overlayHWND);
	Sleep(300);
	if (overlayHWND == 0)
	{
		printf(XorStr("Can't find the overlay\n"));
		Sleep(1000);
		exit(0);
	}

	HDC hDC = ::GetWindowDC(NULL);
	width = ::GetDeviceCaps(hDC, HORZRES);
	height = ::GetDeviceCaps(hDC, VERTRES);

	running = true;

	// Initialize Direct3D
	if (!CreateDeviceD3D(overlayHWND))
	{
		CleanupDeviceD3D();
		return 1;
	}

	// Show the window
	::ShowWindow(overlayHWND, SW_SHOWDEFAULT);
	::UpdateWindow(overlayHWND);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Setup Dear ImGui style
	ImGui::StyleColorsCustom();
	ImGui::GetStyle().WindowMinSize = ImVec2(1, 1);

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(overlayHWND);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ClickThrough(true);
	while (running)
	{
		HWND wnd = GetWindow(GetForegroundWindow(), GW_HWNDPREV);
		if (use_nvidia)
		{
			if (GetWindowLong(overlayHWND, GWL_STYLE) != nv_edit)
				SetWindowLong(overlayHWND, GWL_STYLE, nv_edit);
			if (show_menu)
			{
				ClickThrough(false);
			}
			else
			{
				if (GetWindowLong(overlayHWND, GWL_EXSTYLE) != nv_ex_edit)
					SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_edit);
				ClickThrough(true);
			}
		}
		if (wnd != overlayHWND)
		{
			SetWindowPos(overlayHWND, wnd, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
			::UpdateWindow(overlayHWND);
		}

		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (IsKeyDown(VK_LBUTTON) && !k_leftclick)
		{
			io.MouseDown[0] = true;
			k_leftclick = true;
		}
		else if (!IsKeyDown(VK_LBUTTON) && k_leftclick)
		{
			io.MouseDown[0] = false;
			k_leftclick = false;
		}

		if ((IsKeyDown(VK_INSERT) || IsKeyDown(VK_HOME)) && !k_ins )
		{
			show_menu = !show_menu;
			ClickThrough(!show_menu);
			k_ins = true;
		}
		else if (!(IsKeyDown(VK_INSERT) || IsKeyDown(VK_HOME)) && k_ins)
		{
			k_ins = false;
		}
		if (show_menu)
			RenderMenu();
		else {
			WindowInfo();
			RenderEsp();
		}
		// Rendering
		ImGui::EndFrame();
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	ClickThrough(true);

	CleanupDeviceD3D();
	::DestroyWindow(overlayHWND);
	return 0;
}

void UI::Start()
{
	DWORD ThreadID;
	CreateThread(NULL, 0, StaticMessageStart, (void*)this, 0, &ThreadID);
}

void UI::Clear()
{
	running = 0;
	Sleep(50);
	if (use_nvidia)
	{
		SetWindowLong(overlayHWND, GWL_STYLE, nv_default);
		SetWindowLong(overlayHWND, GWL_EXSTYLE, nv_ex_default);
	}
}

int UI::getWidth()
{
	return width;
}

int UI::getHeight()
{
	return height;
}

// Helper functions

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer)
	{
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void UI::DrawLine(ImVec2 a, ImVec2 b, ImColor color, float width)
{
	ImGui::GetWindowDrawList()->AddLine(a, b, color, width);
}

void UI::DrawBox(ImColor color, float x, float y, float w, float h)
{
	DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.0f);
	DrawLine(ImVec2(x, y), ImVec2(x, y + h), color, 1.0f);
	DrawLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.0f);
	DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, 1.0f);
}

void UI::DrawCornerBox(ImVec2 rect, ImVec2 size, ImColor color, float width)
{
	size.x - 5;
	float lineW = (size.x / 5);
	float lineH = (size.y / 6);
	float lineT = 1;

	//outline
	DrawLine(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x + lineW, rect.y - lineT), color, width); //top left
	DrawLine(ImVec2(rect.x - lineT, rect.y - lineT), ImVec2(rect.x - lineT, rect.y + lineH), color, width);
	DrawLine(ImVec2(rect.x - lineT, rect.y + size.y - lineH), ImVec2(rect.x - lineT, rect.y + size.y + lineT), color, width); //bot left
	DrawLine(ImVec2(rect.x - lineT, rect.y + size.y + lineT), ImVec2(rect.x + lineW, rect.y + size.y + lineT), color, width);
	DrawLine(ImVec2(rect.x + size.x - lineW, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y - lineT), color, width); // top right
	DrawLine(ImVec2(rect.x + size.x + lineT, rect.y - lineT), ImVec2(rect.x + size.x + lineT, rect.y + lineH), color, width);
	DrawLine(ImVec2(rect.x + size.x + lineT, rect.y + size.y - lineH), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, width); // bot right
	DrawLine(ImVec2(rect.x + size.x - lineW, rect.y + size.y + lineT), ImVec2(rect.x + size.x + lineT, rect.y + size.y + lineT), color, width);

	//inline
	DrawLine(ImVec2(rect.x, rect.y), ImVec2(rect.x, rect.y + lineH), color, width);//top left
	DrawLine(ImVec2(rect.x, rect.y), ImVec2(rect.x + lineW, rect.y), color, width);
	DrawLine(ImVec2(rect.x + size.x - lineW, rect.y), ImVec2(rect.x + size.x, rect.y), color, width); //top right
	DrawLine(ImVec2(rect.x + size.x, rect.y), ImVec2(rect.x + size.x, rect.y + lineH), color, width);
	DrawLine(ImVec2(rect.x, rect.y + size.y - lineH), ImVec2(rect.x, rect.y + size.y), color, width); //bot left
	DrawLine(ImVec2(rect.x, rect.y + size.y), ImVec2(rect.x + lineW, rect.y + size.y), color, width);
	DrawLine(ImVec2(rect.x + size.x - lineW, rect.y + size.y), ImVec2(rect.x + size.x, rect.y + size.y), color, width);//bot right
	DrawLine(ImVec2(rect.x + size.x, rect.y + size.y - lineH), ImVec2(rect.x + size.x, rect.y + size.y), color, width);
}
void UI::DrawCircleFOV(ImVec2 center, float radius, ImColor col, float thick)
{
	ImGui::GetWindowDrawList()->AddCircle(center, radius, col, NULL, thick);
}


void UI::Text(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect)
{
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, color, text_begin, text_end, wrap_width, cpu_fine_clip_rect);
}

void UI::String(ImVec2 pos, ImColor color, const char* text)
{
	Text(pos, color, text, text + strlen(text), 200, 0);
}
void UI::StringOutlined(ImVec2 pos, ImColor color, const char* text, bool centerText)
{
	DrawOutlinedText(pos, color, text, text + strlen(text), 100, 0, true);
}

void UI::DrawOutlinedText(ImVec2 pos, ImColor color, const char* text_begin, const char* text_end, float wrap_width, const ImVec4* cpu_fine_clip_rect, bool centerText)
{
	ImVec2 drawPos = pos;
	ImVec2 textSize = ImGui::CalcTextSize(text_begin);
	if (centerText)
	{

		drawPos.x = pos.x - textSize.x / 2;
		drawPos.y = pos.y - textSize.y;
	}
	UI::RectFilled(pos.x - textSize.x / 2 - 1, pos.y - textSize.y, pos.x + textSize.x / 2 + 1, pos.y, ImColor(0, 0, 0, 180), 30, 15);
	ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), drawPos, color, text_begin);
}
void UI::RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
}

void UI::ProgressBar(float x, float y, float w, float h, int value, int v_max)
{
	ImColor barColor = ImColor(
		min(510 * (v_max - value) / 100, 255),
		min(510 * value / 100, 255),
		25,
		255
	);

	RectFilled(x, y, x + w, y + ((h / float(v_max)) * (float)value), barColor, 0.0f, 0);
}
void UI::DrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color) {
	ImGui::GetWindowDrawList()->AddQuadFilled(p1, p2, p3, p4, color);
}
void UI::DrawHexagon(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col, float thickness)
{
	ImGui::GetWindowDrawList()->AddHexagon(p1, p2, p3, p4, p5, p6, col, thickness);
}
void UI::DrawHexagonFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col)
{
	ImGui::GetWindowDrawList()->AddHexagonFilled(p1, p2, p3, p4, p5, p6, col);
}

void UI::DrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health, float size) {

	int bg_offset = 3 / size;
	int bar_width = 158 / size;
	// 4steps...2*3=6
	// 38*4=152 152+6 = 158
	// 5steps...2*4=8
	// 30*5=150 150+8 = 158
	float max_health = 100.0f / size;
	float shield_step = 25.0f / size;

	int shield_25 = 30 / size;
	//int steps = 5 / size;


	ImVec2 bg1(x - bar_width / 2 - bg_offset, y);
	ImVec2 bg2(bg1.x - 10, bg1.y - 16);
	ImVec2 bg3(bg2.x + 5, bg2.y - 7);
	ImVec2 bg4(bg3.x + bar_width + bg_offset, bg3.y);
	ImVec2 bg5(bg4.x + 11, bg4.y + 18);
	ImVec2 bg6(x + bar_width / 2 + bg_offset, y);
	DrawHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


	ImVec2 h1(bg1.x + 3, bg1.y - 4);
	ImVec2 h2(h1.x - 5, h1.y - 8);
	ImVec2 h3(h2.x + (float)(health / size) / max_health * bar_width, h2.y);
	ImVec2 h4(h1.x + (float)(health / size) / max_health * bar_width, h1.y);
	ImVec2 h3m(h2.x + bar_width, h2.y);
	ImVec2 h4m(h1.x + bar_width, h1.y);
	DrawQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
	DrawQuadFilled(h1, h2, h3, h4, WHITE);


	ImColor shieldCracked(97, 97, 97);
	ImColor shieldCrackedDark(67, 67, 67);

	ImColor shieldCol;
	ImColor shieldColDark; //not used, but the real seer q has shadow inside
	if (armorType == 1) { //white
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	else if (armorType == 2) { //blue
		shieldCol = ImColor(39, 178, 255);
		shieldColDark = ImColor(27, 120, 210);
	}
	else if (armorType == 3) { //purple
		shieldCol = ImColor(206, 59, 255);
		shieldColDark = ImColor(136, 36, 220);
	}
	else if (armorType == 4) { //gold
		shieldCol = ImColor(255, 255, 79);
		shieldColDark = ImColor(218, 175, 49);
	}
	else if (armorType == 5) { //red
		shieldCol = ImColor(219, 2, 2);
		shieldColDark = ImColor(219, 2, 2);
	}
	else {
		shieldCol = ImColor(247, 247, 247);
		shieldColDark = ImColor(164, 164, 164);
	}
	int shield_tmp = shield / size;
	int shield1 = 0;
	int shield2 = 0;
	int shield3 = 0;
	int shield4 = 0;
	int shield5 = 0;
	if (shield_tmp > shield_step) {
		shield1 = shield_step;
		shield_tmp -= shield_step;
		if (shield_tmp > shield_step) {
			shield2 = shield_step;
			shield_tmp -= shield_step;
			if (shield_tmp > shield_step) {
				shield3 = shield_step;
				shield_tmp -= shield_step;
				if (shield_tmp > shield_step) {
					shield4 = shield_step;
					shield_tmp -= shield_step;
					shield5 = shield_tmp;
				}
				else {
					shield4 = shield_tmp;
				}
			}
			else {
				shield3 = shield_tmp;
			}
		}
		else {
			shield2 = shield_tmp;
		}
	}
	else {
		shield1 = shield_tmp;
	}
	ImVec2 s1(h2.x - 1, h2.y - 2);
	ImVec2 s2(s1.x - 3, s1.y - 5);
	ImVec2 s3(s2.x + shield1 / shield_step * shield_25, s2.y);
	ImVec2 s4(s1.x + shield1 / shield_step * shield_25, s1.y);
	ImVec2 s3m(s2.x + shield_25, s2.y);
	ImVec2 s4m(s1.x + shield_25, s1.y);

	ImVec2 ss1(s4m.x + 2, s1.y);
	ImVec2 ss2(s3m.x + 2, s2.y);
	ImVec2 ss3(ss2.x + shield2 / shield_step * shield_25, s2.y);
	ImVec2 ss4(ss1.x + shield2 / shield_step * shield_25, s1.y);
	ImVec2 ss3m(ss2.x + shield_25, s2.y);
	ImVec2 ss4m(ss1.x + shield_25, s1.y);

	ImVec2 sss1(ss4m.x + 2, s1.y);
	ImVec2 sss2(ss3m.x + 2, s2.y);
	ImVec2 sss3(sss2.x + shield3 / shield_step * shield_25, s2.y);
	ImVec2 sss4(sss1.x + shield3 / shield_step * shield_25, s1.y);
	ImVec2 sss3m(sss2.x + shield_25, s2.y);
	ImVec2 sss4m(sss1.x + shield_25, s1.y);

	ImVec2 ssss1(sss4m.x + 2, s1.y);
	ImVec2 ssss2(sss3m.x + 2, s2.y);
	ImVec2 ssss3(ssss2.x + shield4 / shield_step * shield_25, s2.y);
	ImVec2 ssss4(ssss1.x + shield4 / shield_step * shield_25, s1.y);
	ImVec2 ssss3m(ssss2.x + shield_25, s2.y);
	ImVec2 ssss4m(ssss1.x + shield_25, s1.y);

	ImVec2 sssss1(ssss4m.x + 2, s1.y);
	ImVec2 sssss2(ssss3m.x + 2, s2.y);
	ImVec2 sssss3(sssss2.x + shield5 / shield_step * shield_25, s2.y);
	ImVec2 sssss4(sssss1.x + shield5 / shield_step * shield_25, s1.y);
	ImVec2 sssss3m(sssss2.x + shield_25, s2.y);
	ImVec2 sssss4m(sssss1.x + shield_25, s1.y);
	if (max_shield == 50) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield != 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
	}
	else if (max_shield == 75) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (shield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (shield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
	}
	else if (max_shield == 100) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (shield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (shield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (shield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (shield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
	}
	else if (max_shield == 125) {
		if (shield <= 25) {
			if (shield < 25) {
				DrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(s1, s2, s3, s4, shieldCol);

		}
		else if (shield <= 50) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			if (shield < 50) {
				DrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
		}
		else if (shield <= 75) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			if (shield < 75) {
				DrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
		}
		else if (shield <= 100) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			if (shield < 100) {
				DrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
		}
		else if (shield <= 125) {
			DrawQuadFilled(s1, s2, s3, s4, shieldCol);
			DrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
			DrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
			DrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
			if (shield < 125) {
				DrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
			}
			if (shield != 0)
				DrawQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
		}
	}
}
void UI::RainBow()
{
	float rainbowSpeed = 0.00060;
	static float staticHue = 0;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 panelPos = ImGui::GetWindowPos();
	staticHue -= rainbowSpeed;
	if (staticHue < -1.f) staticHue += 1.f;
	for (int i = 0; i < 1200; i++)
	{
		float hue = staticHue + (1.f / (float)1200) * i;
		if (hue < 0.f) hue += 1.0f;
		ImColor cRainbow = ImColor::HSV(hue, 1.f, 1.f);
		draw_list->AddRectFilled(ImVec2(panelPos.x + i, panelPos.y + 2), ImVec2(panelPos.x + i + 1, panelPos.y + 4), cRainbow);
	}
}
ImVec2 addVec2(ImVec2 v1, ImVec2 v2) {
	return ImVec2(v1.x + v2.x, v1.y + v2.y);
}
void UI::DrawFilledArrow(float x, float y, float size, ImColor color, float yaw)
{
	// create arrow at center point
	ImVec2 p1 = ImVec2(0, -size / 2.0f);
	ImVec2 p2 = ImVec2(-size / 3.0f, size / 2.0f);
	ImVec2 p3 = ImVec2(0, size / 5.0f);
	ImVec2 p4 = ImVec2(size / 3.0f, size / 2.0f);

	//fix yaw
	float angle = -yaw; // yaw is inverted
	if (angle < 0)
		angle += 360;
	angle += 90.0f; // yaw is off by 90°
	if (angle > 360)
		angle -= 360;

	//if (isMinimap) {
	//	//add player angle so it  matches with rotated minimap
	//	float pangle = -LP_info.yaw; // yaw is inverted
	//	if (pangle < 0)
	//		pangle += 360;
	//	pangle += v.yaw_off; // yaw is off by 90°
	//	if (pangle > 360)
	//		pangle -= 360;

	//	angle = angle - pangle; //subtract your own yaw(for minimap-rotation)
	//	if (angle < 0)
	//		angle += 360;
	//	if (angle > 360)
	//		angle -= 360;
	//}

	// rotate
	if (angle > 0)
	{
		p1 = rotate(p1, angle);
		p2 = rotate(p2, angle);
		p3 = rotate(p3, angle);
		p4 = rotate(p4, angle);
	}

	// add position
	ImVec2 pos = ImVec2(x, y);
	p1 = addVec2(p1, pos);
	p2 = addVec2(p2, pos);
	p3 = addVec2(p3, pos);
	p4 = addVec2(p4, pos);

	// draw arrow
	DrawQuadFilled(p1, p2, p3, p4, color);
}
class world {
public:
	ImVec2 w1; //origin of point 1
	ImVec2 w2; //origin of point 2
	ImVec2 s1; //screen coord of point 1
	ImVec2 s2; //screen coord of point 2
	float ratioX;
	float ratioY;
	world(ImVec2 w1, ImVec2 s1, ImVec2 w2, ImVec2 s2) {
		this->w1 = w1;
		this->w2 = w2;
		this->s1 = s1;
		this->s2 = s2;
		this->ratioX = (s2.x - s1.x) / (w2.x - w1.x);
		this->ratioY = (s1.y - s2.y) / (w2.y - w1.y);
	}
	ImVec2 getBaseW() {
		return w1;
	}
	ImVec2 getBaseS() {
		return s1;
	}
	float getRatioX() {
		return ratioX;
	}
	float getRatioY() {
		return ratioY;
	}
};

//Battel Royal
world KingsCanyon(ImVec2(26335.312500, -14373.276367), ImVec2(1211, 721), ImVec2(-7143.280762, 19977.578125), ImVec2(796, 296)); //2kome bimyou kamo
world WorldsEdge(ImVec2(-9190.608398, 8443.554688), ImVec2(824, 412), ImVec2(-19529.794922, -8933.173828), ImVec2(707, 608));//1920x1080
//world WorldsEdge(ImVec2(-17133.048828, 30297.880859), ImVec2(979, 222), ImVec2(19616.375000, -42115.308594), ImVec2(1532, 1311));//2560x1440
//world WorldsEdge(ImVec2(-2549.653809, 34636.945312), ImVec2(1640, 156), ImVec2(8851.856445, -42751.222656), ImVec2(1811, 1320));//3440x1440
world Olympus(ImVec2(-27204.990234, -6551.791992), ImVec2(699, 614), ImVec2(8493.144531, 27624.617188), ImVec2(1101, 229)); //to be measured
world StormPoint(ImVec2(-21264.427734, -47086.878906), ImVec2(711, 983), ImVec2(40298.070313, 21163.728516), ImVec2(1321, 306));
world BrokenMoon(ImVec2(-41417.582031, 29679.375000), ImVec2(462, 162), ImVec2(1935.259277, -23597.134766), ImVec2(973, 791));
//Arena
world Overflow(ImVec2(-3344.994629, -4018.093018), ImVec2(552, 431), ImVec2(5039.592773, -4639.289063), ImVec2(1322, 489));
world DropOff(ImVec2(3135.113281, 1654.107666), ImVec2(1151, 603), ImVec2(-2920.918701, 811.240479), ImVec2(722, 663));
world Habitat4(ImVec2(4482.470215, -604.362854), ImVec2(1205, 544), ImVec2(-4464.019043, 593.067688), ImVec2(650, 470));
world Encore(ImVec2(4144.926270, 468.957611), ImVec2(1184, 472), ImVec2(-3791.070313, 3.092307), ImVec2(692, 501));
world PartyCrasher(ImVec2(-3275.972900, 3646.970703), ImVec2(589, 197), ImVec2(1085.708740, -3869.658936), ImVec2(1022, 943));
#ifdef screen_1440
world Olympus(ImVec2(-27204.990234, -6551.791992), ImVec2(932, 818), ImVec2(8493.144531, 27624.617188), ImVec2(1468, 305));
//Battel Royal
world KingsCanyon(ImVec2(26335.312500, -14373.276367), ImVec2(1211, 721), ImVec2(-7143.280762, 19977.578125), ImVec2(796, 296)); //2kome bimyou kamo
world WorldsEdge(ImVec2(-9190.608398, 8443.554688), ImVec2(824, 412), ImVec2(-19529.794922, -8933.173828), ImVec2(707, 608));
world Olympus(ImVec2(-27204.990234, -6551.791992), ImVec2(699, 614), ImVec2(8493.144531, 27624.617188), ImVec2(1101, 229)); //to be measured
world StormPoint(ImVec2(-21264.427734, -47086.878906), ImVec2(711, 983), ImVec2(40298.070313, 21163.728516), ImVec2(1321, 306));
world BrokenMoon(ImVec2(-41417.582031, 29679.375000), ImVec2(462, 162), ImVec2(1935.259277, -23597.134766), ImVec2(973, 791));
//Arena
world Overflow(ImVec2(-3344.994629, -4018.093018), ImVec2(552, 431), ImVec2(5039.592773, -4639.289063), ImVec2(1322, 489));
world DropOff(ImVec2(3135.113281, 1654.107666), ImVec2(1151, 603), ImVec2(-2920.918701, 811.240479), ImVec2(722, 663));
world Habitat4(ImVec2(4482.470215, -604.362854), ImVec2(1205, 544), ImVec2(-4464.019043, 593.067688), ImVec2(650, 470));
world Encore(ImVec2(4144.926270, 468.957611), ImVec2(1184, 472), ImVec2(-3791.070313, 3.092307), ImVec2(692, 501));
world PartyCrasher(ImVec2(-3275.972900, 3646.970703), ImVec2(589, 197), ImVec2(1085.708740, -3869.658936), ImVec2(1022, 943));
#endif // DEBUG

ImVec2 worldToScreenMap(ImVec2 origin) {
	float ratioX;
	float ratioY;
	ImVec2 w1;
	ImVec2 s1;
	if (strncmp(map_name, "mp_rr_tropic_island", 19) == 0) { //Storm Point
		ratioX = StormPoint.ratioX;
		ratioY = StormPoint.ratioY;
		w1 = StormPoint.w1;
		s1 = StormPoint.s1;
	}
	else if (strncmp(map_name, "mp_rr_aqueduct", 14) == 0) { //arena Overflow
		ratioX = Overflow.ratioX;
		ratioY = Overflow.ratioY;
		w1 = Overflow.w1;
		s1 = Overflow.s1;
	}
	else if (strncmp(map_name, "mp_rr_arena_composite", 21) == 0) { //arena DropOff
		ratioX = DropOff.ratioX;
		ratioY = DropOff.ratioY;
		w1 = DropOff.w1;
		s1 = DropOff.s1;
	}
	else if (strncmp(map_name, "mp_rr_arena_habitat", 19) == 0) { //arena Habitat4
		ratioX = Habitat4.ratioX;
		ratioY = Habitat4.ratioY;
		w1 = Habitat4.w1;
		s1 = Habitat4.s1;
	}
	else if (strncmp(map_name, "mp_rr_arena_skygarden", 21) == 0) { //arena Encore
		ratioX = Encore.ratioX;
		ratioY = Encore.ratioY;
		w1 = Encore.w1;
		s1 = Encore.s1;
	}
	else if (strncmp(map_name, "mp_rr_party_crasher", 19) == 0) { //arena PartyCrasher
		ratioX = PartyCrasher.ratioX;
		ratioY = PartyCrasher.ratioY;
		w1 = PartyCrasher.w1;
		s1 = PartyCrasher.s1;
	}
	else if (strncmp(map_name, "mp_rr_divided_moon", 18) == 0) { //BrokenMoon
		ratioX = BrokenMoon.ratioX;
		ratioY = BrokenMoon.ratioY;
		w1 = BrokenMoon.w1;
		s1 = BrokenMoon.s1;
	}
	else if (strncmp(map_name, "mp_rr_canyonlands_hu", 20) == 0) { //KingsCanyon   s14;mu->hu
		ratioX = KingsCanyon.ratioX;
		ratioY = KingsCanyon.ratioY;
		w1 = KingsCanyon.w1;
		s1 = KingsCanyon.s1;
	}
	else if (strncmp(map_name, "mp_rr_desertlands_mu", 20) == 0) { //WorldsEdge
		ratioX = WorldsEdge.ratioX;
		ratioY = WorldsEdge.ratioY;
		w1 = WorldsEdge.w1;
		s1 = WorldsEdge.s1;
	}
	else if (strncmp(map_name, "mp_rr_olympus", 13) == 0) { //Olympus 
		ratioX = Olympus.ratioX;
		ratioY = Olympus.ratioY;
		w1 = Olympus.w1;
		s1 = Olympus.s1;
	}
	else {
		return ImVec2(0, 0);
	}

	//difference from location 1
	float world_diff_x = origin.x - w1.x;
	float world_diff_y = origin.y - w1.y;

	//get the screen offsets by applying the ratio
	float scr_diff_x = world_diff_x * ratioX;
	float scr_diff_y = world_diff_y * ratioY;

	//for x, add the offset to the screen x of location 1
	//for y, subtract the offset from the screen y of location 1 (cuz Y is from bottom to up in Apex but it's from up to bottom in screen)
	float pos_x = s1.x + scr_diff_x;
	float pos_y = s1.y - scr_diff_y;

	return ImVec2(pos_x, pos_y);
}
void UI::drawPlayerOnFullRadar(float x, float y, ImColor color, float yaw, int teamId) {
	ImVec2 pos = worldToScreenMap(ImVec2(x, y));
	if (v.radar_arrow_shadow) {
		DrawFilledArrow(pos.x, pos.y, v.radar_arrow_size + v.radar_shadow_size, ImColor((int)color.Value.x * 255, (int)color.Value.y * 255, (int)color.Value.z * 255, v.radar_shadow_alpha), yaw);
		DrawFilledArrow(pos.x, pos.y, v.radar_arrow_size + v.radar_shadow_size, ImColor(v.radar_shadow_color, v.radar_shadow_color, v.radar_shadow_color, v.radar_shadow_alpha), yaw);
	}
	DrawFilledArrow(pos.x, pos.y, v.radar_arrow_size, color, yaw);
	if (v.radar_show_teamId) {
		StringOutlined(ImVec2(pos.x, pos.y - v.radar_teamId_y), color, std::to_string(teamId).c_str(),true);
	}


}





