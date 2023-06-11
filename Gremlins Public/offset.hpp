#pragma once
#include <Windows.h>

namespace offset
{
	static  uintptr_t cl_entitylist = 0x1d87e88;
	static  uintptr_t localplayer = 0x02139480 + 0x8;

	static  uintptr_t namelist = 0xbd27ba0;
	static  uintptr_t nameindex = 0x38;

	static  uintptr_t viewrender = 0x7400bd0;
	static  uintptr_t viewmatrix = 0x11a350;

	static  uintptr_t stdiohdr = 0x10f0; //CBaseAnimating!m_pStudioHdr

	static  uintptr_t bones = 0xEE0;
	static  uintptr_t team = 0x044c;
	static  uintptr_t life_state = 0x0798;
	static  uintptr_t bleed_out_state = 0x2750;
	static  uintptr_t health = 0x043c;
	static  uintptr_t shield_health = 0x0170;
	static  uintptr_t shield_health_max = 0x174;
	static  uintptr_t shield_type = 0x464c;
	static  uintptr_t visivle_time = 0x1a80;
	static  uintptr_t weapon = 0x1a24;
	static  uintptr_t weapon2 = 0x1a34;
	static  uintptr_t origin = 0x14C;
	static  uintptr_t view_angle = 0x25bc - 0x14;
	static  uintptr_t dynamic_angles = view_angle - 0x10;
	static  uintptr_t cameraangle = 0x1f58;
	static  uintptr_t aim_punch = 0x24c0;
	static  uintptr_t abs_velocity = 0x0140;
	static  uintptr_t angle_yaw = 0x22c4 - 0x8;
	static  uintptr_t signame = 0x589;
	static	uintptr_t crosshair_last = 0x1a88;

	static  uintptr_t bullet_speed = 0x1A88 + 0x04c8;
	static  uintptr_t bullet_gravity = 0x1A88 + 0x04d0;
	static  uintptr_t weapon_name_index = 0x1870;
	static  uintptr_t zoom_fov = 0x00b8;

	static  uintptr_t in_attack = 0x074034b0;
}