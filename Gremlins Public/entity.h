#pragma once
#include <string>

#include "interface.h"

#define OFFSET(ent, type, func, offset) type func { return mem->RPM<type>( ent + offset ); } 

struct GlowMode
{
	int8_t GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

class weapon_t
{
public:
	weapon_t(uintptr_t _ptr = 0) {
		this->ptr = _ptr;
	}

	uintptr_t ptr;

	OFFSET(ptr, float, bullet_speed(), offset::bullet_speed)
	OFFSET(ptr, float, bullet_gravity(), offset::bullet_gravity)
	OFFSET(ptr, int, weapon_name_index(), offset::weapon_name_index)
	OFFSET(ptr, float, zoom_fov(), offset::zoom_fov)
};

struct HitBoxManager
{
	Vector3 ScreenHeadBone{};
	Vector3 ScreenNeckBone{};
	Vector3 ScreenChestBone{};
	Vector3 ScreenWaistBone{};
	Vector3 ScreenBotmBone{};

	Vector3 ScreenLeftshoulderBone{};
	Vector3 ScreenLeftelbowBone{};
	Vector3 ScreenLeftHandBone{};
	Vector3 ScreenRightshoulderBone{};
	Vector3 ScreenRightelbowBone{};
	Vector3 ScreenRightHandBone{};

	Vector3 ScreenLeftThighsBone{};
	Vector3 ScreenLeftkneesBone{};
	Vector3 ScreenLeftlegBone{};
	Vector3 ScreenRightThighsBone{};
	Vector3 ScreenRightkneesBone{};
	Vector3 ScreenRightlegBone{};
};

class entity_t
{
public:
	entity_t(DWORD_PTR _ptr) {
		this->ptr = _ptr;

		if (_ptr && is_player())
		{
			this->team_v = team();
			this->life_state_v = life_state();
			this->bleed_out_state_v = bleed_out_state();
			this->health_v = health();
			this->shield_health_v = shield_health();
			this->shield_health_max_v = shield_health_max();
			this->shield_type_v = shield_type();
			this->visible_time_v = visible_time();
			this->weapon_v = weapon();
			this->weapon2_v = weapon2();
			this->origin_v = origin();
			this->view_angle_v = view_angle();
			this->dynamic_angles_v = dynamic_angles();
			this->cameraangle_v = cameraangle();
			this->aim_punch_v = aim_punch();
			this->abs_velocity_v = abs_velocity();
			this->angle_yaw_v = angle_yaw();
			this->signame_v = signame();
			this->crosshair_last_v = crosshair_last();

			this->pname = get_player_name();

			this->active_weapon_t = get_active_weapon();

			if(setting::SkeltonESP)
				this->hitbox = get_hitbox();
		}
	}

	uintptr_t ptr;

	int team_v;
	int life_state_v;
	int bleed_out_state_v;
	int health_v;
	int shield_health_v;
	int shield_health_max_v;
	int shield_type_v;
	float visible_time_v;
	uintptr_t weapon_v;
	uintptr_t weapon2_v;
	Vector3 origin_v;
	Vector3 view_angle_v;
	Vector3 dynamic_angles_v;
	Vector3 cameraangle_v;
	Vector3 aim_punch_v;
	Vector3 abs_velocity_v;
	float angle_yaw_v;
	uintptr_t signame_v;

	float crosshair_last_v;

	weapon_t active_weapon_t;

	bool is_visible_v = false;

	std::string pname;

	HitBoxManager hitbox;

	OFFSET(ptr, int, team(), offset::team);
	OFFSET(ptr, int, life_state(), offset::life_state);
	OFFSET(ptr, int, bleed_out_state(), offset::bleed_out_state)
	OFFSET(ptr, int, health(), offset::health)
	OFFSET(ptr, int, shield_health(), offset::shield_health)
	OFFSET(ptr, int, shield_health_max(), offset::shield_health_max)
	OFFSET(ptr, int, shield_type(), offset::shield_type)
	OFFSET(ptr, float, visible_time(), offset::visivle_time)
	OFFSET(ptr, uintptr_t, weapon(), offset::weapon)
	OFFSET(ptr, uintptr_t, weapon2(), offset::weapon2)
	OFFSET(ptr, Vector3, origin(), offset::origin)
	OFFSET(ptr, Vector3, view_angle(), offset::view_angle)
	OFFSET(ptr, Vector3, dynamic_angles(), offset::dynamic_angles)
	OFFSET(ptr, Vector3, cameraangle(), offset::cameraangle)
	OFFSET(ptr, Vector3, aim_punch(), offset::aim_punch)
	OFFSET(ptr, Vector3, abs_velocity(), offset::abs_velocity)
	OFFSET(ptr, float, angle_yaw(), offset::angle_yaw)
	OFFSET(ptr, uintptr_t, signame(), offset::signame)
	OFFSET(ptr, float, crosshair_last(), offset::crosshair_last)

	bool is_visible(int index);
	bool is_player();
	bool is_down();
	bool is_alive();
	bool is_valid(entity_t);
	bool is_spectator(float);

	weapon_t get_active_weapon();
	static uintptr_t get_client_entity(const int index);
	static entity_t get_local_player();

	Vector3 get_bone_position(uint32_t);

	float get_yaw_angle();

	std::string get_player_name();

	void set_glow(Vector3 color);

	HitBoxManager get_hitbox();
};