#pragma once
#include <Windows.h>
#include "ImGui/imgui.h"

#include "setting.hpp"
#include "offset.hpp"

#include "driver.h"

#include "vector.h"

#define OFFSET(ent, type, func, offset) type func { return rtcore.rpm<type>( ent + offset ); } 

namespace utils
{
	static std::vector<std::string> SpectatorName;
	static std::vector<int> SpectatorTime;

	static int bones[] = { 7, 5,2, 3, 8 };

	static ImVec2 MainWindowPos = { 100,100 };

	static int MenuTab = 1;

	static int select_config = -1;

	static Vector3 WndSize = { (float)GetSystemMetrics(SM_CXSCREEN) ,(float)GetSystemMetrics(SM_CYSCREEN) , NULL };

	static ImFont* iconesp;

	static ImFont* iconaim;

	static ImFont* iconcolor;

	static ImFont* iconother;

	int GetUnixTimestamp();

	bool WorldToScreen(MatrixA, Vector3, Vector3&);

	MatrixA Get_ViewMatrix();

	Vector3 VAR_Subtracts(Vector3, Vector3);
	Vector3 VAR_toAngless(Vector3 const&);
	Vector3 VAR_toVectors(Vector3 const&);
	double VAR_dots(Vector3 const&, Vector3 const&);
	Vector3 VAR_Adds(Vector3, Vector3);
	Vector3 VAR_scales(float, Vector3 const&);
	Vector3 VAR_units(Vector3 const&);
	Vector3 Var_CalcAngless(Vector3&, Vector3&);
	float VAR_deltaDistances(Vector3, Vector3);
	float biggestAngleChanges(Vector3, Vector3);

	float GetFov(const Vector3&, const Vector3&);

	float GetHealthPercent(int Health);

	float GetShieldPercent(int Shield);
	Vector3 Subtract(Vector3 src, Vector3 dst);
	float Magnitude(Vector3 vec);
	float DeltaDistance(Vector3 src, Vector3 dst);
}