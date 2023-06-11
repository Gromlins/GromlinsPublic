#include "Overlay.h"
#include "interface.h"
#include "images.h"


ImFont* info = nullptr;
ImFont* iconfont = nullptr;
ImFont* pFont = nullptr;
ImDrawList* draw;

namespace Globals {
	HWND gWnd;
	HWND hWnd;
	int ClientWidth;
	int ClientHeight;
	int ClientCenterX;
	int ClientCenterY;
	bool isTop;
	bool isCursorVis;
	POINT cursorPos;
	//LPDIRECT3D9 g_pD3D;
	//IDirect3D9Ex* p_Object;
	//IDirect3DDevice9Ex* p_Device;
	//LPDIRECT3DDEVICE9 g_pd3dDevice;
	//D3DPRESENT_PARAMETERS g_d3dpp;
	//MARGINS margin;
	//RECT rc;
	//MSG message;

	D3DXMATRIX ViewMatrix;
	D3DXMATRIX ProjViewMatrix;

	ImFont* menuFont;
	ImFont* tabFont;
	ImFont* controlFont;
	ImFont* boldMenuFont;
	IDirect3DTexture9* menuBg;

	//static ImDrawList* drawList;

	POINT windowWH;
}

IDirect3D9Ex* p_Object;
IDirect3DDevice9Ex* p_Device;
D3DPRESENT_PARAMETERS p_Params;
RECT p_rect;
MARGINS p_margin = { -1 };
MSG p_message;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//LRESULT CALLBACK Overlay::WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
//		return true;
//
//	switch (Message)
//	{
//	case WM_SIZE:
//		if (Globals::p_Device != NULL && wParam != SIZE_MINIMIZED)
//		{
//			ImGui_ImplDX9_InvalidateDeviceObjects();
//			p_Params.BackBufferWidth = LOWORD(lParam);
//			Globals::g_d3dpp.BackBufferHeight = HIWORD(lParam);
//			HRESULT hr = Globals::p_Device->Reset(&Globals::g_d3dpp);
//			if (hr == D3DERR_INVALIDCALL)
//				IM_ASSERT(0);
//			ImGui_ImplDX9_CreateDeviceObjects();
//		}
//		break;
//
//	case WM_DESTROY:
//		DirectXShutdown();
//		PostQuitMessage(0);
//		return 0;
//		break;
//
//	default:
//		return DefWindowProcW(hWnd, Message, wParam, lParam);
//		break;
//	}
//	return 0;
//}

LRESULT CALLBACK Overlay::WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;

	case WM_DESTROY:
		DirectXShutdown();
		PostQuitMessage(0);
		return 0;
		break;

	default:
		return DefWindowProcW(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}


//void Overlay::HandleMessages(std::wstring lClassName, std::wstring lWindowName)
//{
//	Globals::hWnd = CreateWindowExW(/*WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED*/NULL, lClassName.c_str(), lWindowName.c_str(), WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, 0, 0);
//	SetWindowLongA(Globals::hWnd, GWL_EXSTYLE, (WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
//	DwmExtendFrameIntoClientArea(Globals::hWnd, &Globals::margin);
//	ShowWindow(Globals::hWnd, SW_SHOW);
//	UpdateWindow(Globals::hWnd);
//
//	while (true)
//	{
//		if (PeekMessageW(&Globals::message, Globals::hWnd, 0, 0, PM_REMOVE))
//		{
//			DispatchMessageW(&Globals::message);
//			TranslateMessage(&Globals::message);
//		}
//
//		HWND hwnd_active = GetForegroundWindow();
//
//		if (hwnd_active == Globals::gWnd)
//		{
//			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
//			SetWindowPos(Globals::hWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//		}
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}

void Overlay::HandleMessages(std::wstring lClassName, std::wstring lWindowName)
{
	Globals::hWnd = CreateWindowExW(/*WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED*/NULL, lClassName.c_str(), lWindowName.c_str(), WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, 0, 0, 0);
	SetWindowLongA(Globals::hWnd, GWL_EXSTYLE, (WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT));
	DwmExtendFrameIntoClientArea(Globals::hWnd, &p_margin);
	ShowWindow(Globals::hWnd, SW_SHOW);
	UpdateWindow(Globals::hWnd);

	while (true)
	{
		if (PeekMessageW(&p_message, Globals::hWnd, 0, 0, PM_REMOVE))
		{
			DispatchMessageW(&p_message);
			TranslateMessage(&p_message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == Globals::gWnd)
		{
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Globals::hWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		Sleep(2);
	}
}

void Overlay::InputHandler() {
	for (int i = 0; i < 5; i++) ImGui::GetIO().MouseDown[i] = false;
	int button = -1;
	if (GetAsyncKeyState(VK_LBUTTON)) button = 0;
	if (button != -1) ImGui::GetIO().MouseDown[button] = true;
}

bool Overlay::InitWindow()
{
	// create a window class
	std::string r1 = es("valorant");
	std::string r2 = es("RiotGames");

	std::wstring lClassName = std::wstring(r1.begin(), r1.end());
	std::wstring lWindowName = std::wstring(r2.begin(), r2.end());

	WNDCLASSEXW wClass;
	wClass.cbSize = sizeof(WNDCLASSEXW);
	wClass.style = 0;
	wClass.lpfnWndProc = WinProc;
	wClass.cbClsExtra = NULL;
	wClass.cbWndExtra = NULL;
	wClass.hInstance = nullptr;
	wClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wClass.hCursor = LoadCursor(0, IDC_ARROW);
	wClass.hbrBackground = nullptr;
	wClass.lpszMenuName = lWindowName.c_str();
	wClass.lpszClassName = lClassName.c_str();
	wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);

	if (!RegisterClassExW(&wClass))
		return false;

	std::thread (HandleMessages, lClassName, lWindowName).detach();

	return true;
}

bool Overlay::DirectXInit()
{
	// initialize directx
	while (!Globals::hWnd) {}

	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		return false;

	GetClientRect(Globals::gWnd, &p_rect);

	Globals::windowWH = { p_rect.right - p_rect.left, p_rect.bottom - p_rect.top };

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = Globals::hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Globals::windowWH.x;
	p_Params.BackBufferHeight = Globals::windowWH.y;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Globals::hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
		return false;

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();

	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::StyleColorsDark();

	style->Alpha = 1.f;
	style->WindowRounding = 5;
	style->FramePadding = ImVec2(4, 3);
	style->WindowPadding = ImVec2(0, 0);
	style->ItemInnerSpacing = ImVec2(4, 4);
	style->ItemSpacing = ImVec2(8, 0);
	style->FrameRounding = 12;
	style->ScrollbarSize = 2.f;
	style->ScrollbarRounding = 12.f;
	style->PopupRounding = 4.f;
	style->Rounding = 11.f;


	//ImVec4* colors = ImGui::GetStyle().Colors;
	//colors[ImGuiCol_ChildBg] = ImColor(235, 235, 235, 0);
	//colors[ImGuiCol_WindowBg] = ImColor(235, 235, 235, 255);
	//colors[ImGuiCol_Text] = ImColor(25, 24, 25, 255);
	//colors[ImGuiCol_ChildBg] = ImColor(255, 255, 255, 255);
	//colors[ImGuiCol_Border] = ImVec4(255, 255, 255, 0);
	//colors[ImGuiCol_FrameBg] = ImColor(255, 255, 255, 255);
	//colors[ImGuiCol_FrameBgActive] = ImColor(255, 255, 255, 255);
	//colors[ImGuiCol_FrameBgHovered] = ImColor(255, 255, 255, 255);
	//colors[ImGuiCol_Header] = ImColor(141, 142, 144, 255);
	//colors[ImGuiCol_HeaderActive] = ImColor(141, 142, 144, 255);
	//colors[ImGuiCol_HeaderHovered] = ImColor(141, 142, 144, 255);
	////colors[ImGuiCol_PopupBg] = ImColor(141, 142, 144, 255);
	//colors[ImGuiCol_PopupBg] = ImColor(255, 255, 255, 255);
	//colors[ImGuiCol_TextSelectedBg] = ImColor(231, 231, 231, 255);
	//colors[ImGuiCol_Button] = ImColor(160, 30, 30, 255);
	//colors[ImGuiCol_ButtonHovered] = ImColor(190, 45, 35, 255);
	//colors[ImGuiCol_ButtonActive] = ImColor(220, 60, 40, 255);
	//colors[ImGuiCol_ScrollbarGrab] = ImVec4(110 / 255.f, 122 / 255.f, 200 / 255.f, 1.f);
	//colors[ImGuiCol_SliderGrab] = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);
	//colors[ImGuiCol_CheckMark] = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);

	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = 1;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x044F, // Cyrillic
		0,
	};

	io.IniFilename = NULL;
	info = iconfont = pFont = io.Fonts->AddFontFromFileTTF(es("C:\\Windows\\Fonts\\tahomabd.ttf"), 13, nullptr, io.Fonts->GetGlyphRangesJapanese());

	//utils::iconesp = io.Fonts->AddFontFromMemoryTTF(&image_icon_esp, sizeof image_icon_esp, 24, NULL, io.Fonts->GetGlyphRangesCyrillic());

	//ico_combo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 19, NULL, io.Fonts->GetGlyphRangesCyrillic());

	//ico_button = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 25, NULL, io.Fonts->GetGlyphRangesCyrillic());

	//ico_grande = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 40, NULL, io.Fonts->GetGlyphRangesCyrillic());

	ImGui_ImplWin32_Init(Globals::hWnd);
	ImGui_ImplDX9_Init(p_Device);

	return true;
}

void Overlay::DirectXShutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	p_Object->Release();
	p_Device->Release();

	DestroyWindow(Globals::hWnd);
}

void Overlay::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	p_Device->Reset(&p_Params);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void hDrawLine(const ImVec2& from, const ImVec2& to, const Vector4& color, float thickness) {
	ImGui::GetForegroundDrawList()->AddLine(from, to, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }));
}
void hDrawRect(const ImVec2& from, const ImVec2& to, const Vector4& color, float rounding, uint32_t roundingCornersFlags, float thickness) {
	ImGui::GetForegroundDrawList()->AddRect(from, to, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), rounding, roundingCornersFlags, thickness);
}
void hDrawRectFilled(const ImVec2& from, const ImVec2& to, const Vector4& color, float rounding, uint32_t roundingCornersFlags) {
	ImGui::GetForegroundDrawList()->AddRectFilled(from, to, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), rounding, roundingCornersFlags);
}
void DrawFilledRectangle(int x, int y, int w, int h, Vector4 color)
{
	hDrawRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 1);
}
void hDrawText(const ImVec2& from, const char* text, float size, const Vector4& color) {
	ImGui::GetForegroundDrawList()->AddText(pFont, size, from, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), text);
}
void hDrawTexts(const ImVec2& from, std::string text, float size, const Vector4& color) {
	ImGui::GetForegroundDrawList()->AddText(pFont, size, from, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), text.c_str());
}
void hDrawTextOutlined(const ImVec2& from, const char* text, float size, const Vector4& color)
{
	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(from.x - 1, from.y - 1), ImGui::GetColorU32({ 0, 0, 0, 1 }), text);
	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(from.x + 1, from.y - 1), ImGui::GetColorU32({ 0, 0, 0, 1 }), text);
	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(from.x - 1, from.y + 1), ImGui::GetColorU32({ 0, 0, 0, 1 }), text);
	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(from.x + 1, from.y + 1), ImGui::GetColorU32({ 0, 0, 0, 1 }), text);
	ImGui::GetForegroundDrawList()->AddText(pFont, size, from, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), text);
}

void hDrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color) {
    ImGui::GetForegroundDrawList()->AddQuadFilled(p1, p2, p3, p4, color);
}
void hDrawHexagon(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col, float thickness)
{
    ImGui::GetForegroundDrawList()->AddHexagon(p1, p2, p3, p4, p5, p6, col, thickness);
}
void hDrawHexagonFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col)
{
    ImGui::GetForegroundDrawList()->AddHexagonFilled(p1, p2, p3, p4, p5, p6, col);
}

void hDrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health)
{

    int bg_offset = 3;
    int bar_width = 158;
    // 4steps...2*3=6
    // 38*4=152 152+6 = 158
    // 5steps...2*4=8
    // 30*5=150 150+8 = 158
    float max_health = 100.0f;
    float shield_step = 25.0f;

    int shield_25 = 30;
    int steps = 5;
    //if (strncmp(gamemode, "arena", 5) == 0 || armorType == 4) { //not EVO shield (arena or gold armor)
    //    steps = 4;
    //    shield_25 = 38;
    //}

    ImVec2 bg1(x - bar_width / 2 - bg_offset, y);
    ImVec2 bg2(bg1.x - 10, bg1.y - 16);
    ImVec2 bg3(bg2.x + 5, bg2.y - 7);
    ImVec2 bg4(bg3.x + bar_width + bg_offset, bg3.y);
    ImVec2 bg5(bg4.x + 11, bg4.y + 18);
    ImVec2 bg6(x + bar_width / 2 + bg_offset, y);
    hDrawHexagonFilled(bg1, bg2, bg3, bg4, bg5, bg6, ImColor(0, 0, 0, 120));


    ImVec2 h1(bg1.x + 3, bg1.y - 4);
    ImVec2 h2(h1.x - 5, h1.y - 8);
    ImVec2 h3(h2.x + (float)health / max_health * bar_width, h2.y);
    ImVec2 h4(h1.x + (float)health / max_health * bar_width, h1.y);
    ImVec2 h3m(h2.x + bar_width, h2.y);
    ImVec2 h4m(h1.x + bar_width, h1.y);
    hDrawQuadFilled(h1, h2, h3m, h4m, ImColor(10, 10, 30, 60));
    hDrawQuadFilled(h1, h2, h3, h4, ImColor(255, 255, 255, 255));


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
    int shield_tmp = shield;
    int shield1 = 0;
    int shield2 = 0;
    int shield3 = 0;
    int shield4 = 0;
    int shield5 = 0;
    if (shield_tmp > 25) {
        shield1 = 25;
        shield_tmp -= 25;
        if (shield_tmp > 25) {
            shield2 = 25;
            shield_tmp -= 25;
            if (shield_tmp > 25) {
                shield3 = 25;
                shield_tmp -= 25;
                if (shield_tmp > 25) {
                    shield4 = 25;
                    shield_tmp -= 25;
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
                hDrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(s1, s2, s3, s4, shieldCol);

        }
        else if (shield <= 50) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield != 50) {
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        }
    }
    else if (max_shield == 75) {
        if (shield <= 25) {
            if (shield < 25) {
                hDrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(s1, s2, s3, s4, shieldCol);

        }
        else if (shield <= 50) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield < 50) {
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        }
        else if (shield <= 75) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            if (shield < 75) {
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
        }
    }
    else if (max_shield == 100) {
        if (shield <= 25) {
            if (shield < 25) {
                hDrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(s1, s2, s3, s4, shieldCol);

        }
        else if (shield <= 50) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield < 50) {
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        }
        else if (shield <= 75) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            if (shield < 75) {
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
        }
        else if (shield <= 100) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            hDrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            if (shield < 100) {
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
        }
    }
    else if (max_shield == 125) {
        if (shield <= 25) {
            if (shield < 25) {
                hDrawQuadFilled(s1, s2, s3m, s4m, shieldCracked);
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                hDrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(s1, s2, s3, s4, shieldCol);

        }
        else if (shield <= 50) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            if (shield < 50) {
                hDrawQuadFilled(ss1, ss2, ss3m, ss4m, shieldCracked);
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                hDrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
        }
        else if (shield <= 75) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            if (shield < 75) {
                hDrawQuadFilled(sss1, sss2, sss3m, sss4m, shieldCracked);
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                hDrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
        }
        else if (shield <= 100) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            hDrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            if (shield < 100) {
                hDrawQuadFilled(ssss1, ssss2, ssss3m, ssss4m, shieldCracked);
                hDrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
        }
        else if (shield <= 125) {
            hDrawQuadFilled(s1, s2, s3, s4, shieldCol);
            hDrawQuadFilled(ss1, ss2, ss3, ss4, shieldCol);
            hDrawQuadFilled(sss1, sss2, sss3, sss4, shieldCol);
            hDrawQuadFilled(ssss1, ssss2, ssss3, ssss4, shieldCol);
            if (shield < 125) {
                hDrawQuadFilled(sssss1, sssss2, sssss3m, sssss4m, shieldCracked);
            }
            if (shield != 0)
                hDrawQuadFilled(sssss1, sssss2, sssss3, sssss4, shieldCol);
        }
    }
}

//void iDrawLine(int x, int y, int x2, int y2, uint32_t color, float thick) {
//	float a = (color >> 24) & 0xff;
//	float b = (color >> 16) & 0xff;
//	float g = (color >> 8) & 0xff;
//	float r = (color) & 0xff;
//	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x2, y2), ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), thick);
//}
//void iDrawBox(int x, int y, int w, int h, uint32_t color, float thick)
//{
//	float a = (color >> 24) & 0xff;
//	float b = (color >> 16) & 0xff;
//	float g = (color >> 8) & 0xff;
//	float r = (color) & 0xff;
//	ImU32 Color = ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255));
//	iDrawLine(x, y, x + w, y, Color, thick);
//	iDrawLine(x, y, x, y + h, Color, thick);
//	iDrawLine(x, y + h, x + w, y + h, Color, thick);
//	iDrawLine(x + w, y, x + w, y + h, Color, thick);
//}
//void iDrawFillBox(int x, int y, int w, int h, uint32_t color) {
//	float a = (color >> 24) & 0xff;
//	float b = (color >> 16) & 0xff;
//	float g = (color >> 8) & 0xff;
//	float r = (color) & 0xff;
//	ImU32 Color = ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255));
//	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), Color);
//}
//void iDrawText(int x, int y, uint32_t color, float size, const char* format, ...) {
//	float a = (color >> 24) & 0xff;
//	float b = (color >> 16) & 0xff;
//	float g = (color >> 8) & 0xff;
//	float r = (color) & 0xff;
//	ImU32 Color = ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255));
//	va_list argptr;
//	va_start(argptr, format);
//
//	char buffer[2048];
//	vsprintf_s(buffer, format, argptr);
//
//	va_end(argptr);
//	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(x, y), Color, buffer);
//}
//void iDrawTextOutlined(int x, int y, uint32_t color, float size, const char* format, ...) {
//	float a = (color >> 24) & 0xff;
//	float b = (color >> 16) & 0xff;
//	float g = (color >> 8) & 0xff;
//	float r = (color) & 0xff;
//	ImU32 Color = ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255));
//	va_list argptr;
//	va_start(argptr, format);
//
//	char buffer[2048];
//	vsprintf_s(buffer, format, argptr);
//
//	va_end(argptr);
//	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(x - 1, y - 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
//	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(x + 1, y - 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
//	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(x - 1, y + 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
//	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(x + 1, y + 1), D3DCOLOR_ARGB(255, 0, 0, 0), buffer);
//	ImGui::GetForegroundDrawList()->AddText(pFont, size, ImVec2(x, y), Color, buffer);
//}
//void iDrawImage(void* Image, const ImVec2& Pos, const ImVec2& Size, unsigned int color) {
//	ImGui::GetForegroundDrawList()->AddImage(Image, Pos, ImVec2(Pos.x + Size.x, Pos.y + Size.y), ImVec2(0, 0), ImVec2(1, 1), color);
//}
//
//void SkeltonNum(Vector2 a1, Vector2 a2, int thickness, D3DCOLOR color)
//{
//	ImGui::GetForegroundDrawList()->AddLine(ImVec2(a1.x, a1.y), ImVec2(a2.x, a2.y), color, thickness);
//}
//
void hDrawCircle(int x, int y, float radius, float rez ,float think, const Vector4& color)
{
    ImGui::GetForegroundDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::GetColorU32({ color.x, color.y, color.z, color.w }), (int)rez, think);
}