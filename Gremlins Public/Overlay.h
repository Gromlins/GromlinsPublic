#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <Dwmapi.h>
#include <array>
#include <thread>


#pragma comment (lib,"Dwmapi.lib")


#include <d3d9.h>


#pragma comment (lib, "D3D9.lib")
#pragma comment (lib, "D3dx9.lib")


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"

#include "Vector.h"
#include "xorstr.h"

namespace Overlay {
	bool DirectXInit();
	void HandleMessages(std::wstring lClassName, std::wstring lWindowName);
	bool InitWindow();
	void ResetDevice();
	void InputHandler();
	void DirectXShutdown();
	LRESULT WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
}

namespace Globals {
	extern HWND gWnd;
	extern HWND hWnd;
	extern int ClientWidth;
	extern int ClientHeight;
	extern int ClientCenterX;
	extern int ClientCenterY;
	extern bool isTop;
	extern bool isCursorVis;
	extern POINT cursorPos;
	//extern LPDIRECT3D9 g_pD3D;
	//extern IDirect3D9Ex* p_Object;
	//extern IDirect3DDevice9Ex* p_Device;
	//extern LPDIRECT3DDEVICE9 g_pd3dDevice;
	//extern D3DPRESENT_PARAMETERS g_d3dpp;
	//extern MARGINS margin;
	//extern RECT rc;
	//extern MSG message;

	extern D3DXMATRIX ViewMatrix;
	extern D3DXMATRIX ProjViewMatrix;

	extern ImFont* menuFont;
	extern ImFont* tabFont;
	extern ImFont* controlFont;
	extern ImFont* boldMenuFont;
	extern IDirect3DTexture9* menuBg;

	//static ImDrawList* drawList;

	extern POINT windowWH;
}

extern IDirect3D9Ex* p_Object;
extern IDirect3DDevice9Ex* p_Device;
extern D3DPRESENT_PARAMETERS p_Params;
extern RECT p_rect;
extern MARGINS p_margin;
extern MSG p_message;

//bool CreateDeviceD3D(HWND hWnd);
//void CleanupDeviceD3D();
//void ResetDevice();
//
//void iDrawLine(int x, int y, int x2, int y2, uint32_t color, float thick = 1.0f);
//void iDrawBox(int x, int y, int w, int h, uint32_t color, float thick = 1.0f);
//void iDrawFillBox(int x, int y, int w, int h, uint32_t color);
//void iDrawText(int x, int y, uint32_t color, const char* format, ...);
//void iDrawText(int x, int y, uint32_t color, float size, const char* format, ...);
//void iDrawTextOutlined(int x, int y, uint32_t color, const char* format, ...);
//void iDrawTextOutlined(int x, int y, uint32_t color, float size, const char* format, ...);
//void iDrawImage(void* Image, const ImVec2& Pos, const ImVec2& Size, unsigned int color);

void hDrawLine(const ImVec2& from, const ImVec2& to, const Vector4& color, float thickness);
void hDrawRect(const ImVec2& from, const ImVec2& to, const Vector4& color, float rounding, uint32_t roundingCornersFlags, float thickness);
void hDrawRectFilled(const ImVec2& from, const ImVec2& to, const Vector4& color, float rounding, uint32_t roundingCornersFlags);
void hDrawText(const ImVec2& from, const char* text, float size, const Vector4& color);
void hDrawTextOutlined(const ImVec2& from, const char* text, float size, const Vector4& color);
void hDrawTexts(const ImVec2& from, std::string text, float size, const Vector4& color);
void hDrawCircle(int x, int y, float radius, float rez, float think, const Vector4& color);
void hDrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color);
void hDrawHexagon(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col, float thickness);
void hDrawHexagonFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p5, const ImVec2& p6, ImU32 col);
void hDrawSeerLikeHealth(float x, float y, int shield, int max_shield, int armorType, int health);

extern ImFont* m_Default;