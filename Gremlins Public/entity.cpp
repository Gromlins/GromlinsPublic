#include "entity.h"

float oVisTime[100];
bool entity_t::is_visible(int index)
{
	float visTime = visible_time_v;

	const auto IsVis = visTime > oVisTime[index] || visTime < 0.f && oVisTime[index] > 0.f;

	oVisTime[index] = visTime;

	return IsVis;
}

weapon_t entity_t::get_active_weapon()
{
	const auto weaponid = weapon_v & 0xFFFF;
	if (!weaponid)
		return NULL;

	weapon_t weapon(get_client_entity(weaponid));

	return weapon;
}

uintptr_t entity_t::get_client_entity(const int index) {
	auto entity_list = BaseAddress + offset::cl_entitylist;

	return mem->RPM<uintptr_t>(entity_list + (index << 5));
}

entity_t entity_t::get_local_player() {

	auto local_player = entity_t(mem->RPM<uintptr_t>(BaseAddress + offset::localplayer));

	return local_player;
}

Vector3 entity_t::get_bone_position(uint32_t BoneId)
{
	Vector3 Out{};

	Vector3 origin_vec = origin();

	unsigned long long pBoneArray = mem->RPM<DWORD_PTR>(ptr + offset::bones);

	Out.x = mem->RPM<float>(pBoneArray + 0xCC + (BoneId * 0x30)) + origin_vec.x;
	Out.y = mem->RPM<float>(pBoneArray + 0xDC + (BoneId * 0x30)) + origin_vec.y;
	Out.z = mem->RPM<float>(pBoneArray + 0xEC + (BoneId * 0x30)) + origin_vec.z;

	return Out;
}

bool entity_t::is_player()
{
	return (signame() == 125780153691248);
}

bool entity_t::is_down()
{
	return (bleed_out_state_v > 0);
}

bool entity_t::is_alive()
{
	return (life_state_v == 0);
}

bool entity_t::is_valid(entity_t local)
{
	if (health_v < 0 || health_v > 100 || team_v < 0 || team_v > 100) return false;

	if (setting::TeamCheck)
	{
		if (team_v == local.team_v) return false;
	}

	if (!is_alive()) return false;

	return true;
}

float entity_t::get_yaw_angle()
{
	float angle = angle_yaw();
	float yaw = -angle;
	if (yaw < 0)
		yaw += 360;
	yaw += 90;
	if (yaw > 360)
		yaw -= 360;

	return yaw;
}

std::string entity_t::get_player_name()
{
	uintptr_t nameIndex = mem->RPM<uintptr_t>(ptr + offset::nameindex);
	uintptr_t nameOffset = mem->RPM<uintptr_t>(BaseAddress + offset::namelist + ((nameIndex - 1) << 4));

	return mem->ReadString(nameOffset);
}

bool entity_t::is_spectator(float localyaw)
{
	float targetyaw = angle_yaw_v;

	if (fabs(localyaw - targetyaw) <= DBL_EPSILON * fmax(1, fmax(fabs(localyaw), fabs(targetyaw))) && health() == 0 && get_player_name() != "")
	{
		return true;
	}

	return false;
}

void entity_t::set_glow(Vector3 color)
{
	mem->WPM<int>(ptr + 0x3C8, 1);
	mem->WPM<int>(ptr + 0x3D0, 2);
	mem->WPM<GlowMode>(ptr + 0x2C4, { 126,101,75,90 });
	mem->WPM<float>(ptr + 0x1D0, color.x * 2.5);
	mem->WPM<float>(ptr + 0x1D4, color.y * 2.5);
	mem->WPM<float>(ptr + 0x1D8, color.z * 2.5);
}

HitBoxManager entity_t::get_hitbox()
{
	HitBoxManager DrawPos{};
	int  Bone;

	uint64_t Model = mem->RPM<uint64_t>(ptr + offset::stdiohdr);
	if (!Model) return HitBoxManager();

	uint64_t StudioHdr = mem->RPM<uint64_t>(Model + 0x8);
	if (!StudioHdr) return HitBoxManager();

	uint16_t HitboxCache = mem->RPM<uint16_t>(StudioHdr + 0x34);
	if (!HitboxCache) return HitBoxManager();
	uint64_t HitBoxsArray = StudioHdr + (uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1));

	uint16_t IndexCache = mem->RPM<uint16_t>(HitBoxsArray + 0x4);
	if (!IndexCache) return HitBoxManager();

	int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));

	uint64_t Bones = mem->RPM<uint64_t>(ptr + offset::bones);
	if(!Bones) return HitBoxManager();
		

	//uint16_t Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (1/*hitbox*/ * 0x20));

	//matrix3x4_t BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));


	Vector3 BoneOff;



	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (0 * 0x20));
	matrix3x4_t BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};
	DrawPos.ScreenHeadBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (1 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};	

	DrawPos.ScreenNeckBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (2 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenChestBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (3 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenWaistBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (4 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenBotmBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (6 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenLeftshoulderBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (7 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenLeftelbowBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (8 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenLeftHandBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (9 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenRightshoulderBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (10 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenRightelbowBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (11 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenRightHandBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (12 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenLeftThighsBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (13 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenLeftkneesBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (14 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenLeftlegBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (16 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenRightThighsBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (17 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenRightkneesBone = origin_v + BoneOff;

	
	Bone = mem->RPM<uint16_t>(HitboxIndex + HitBoxsArray + (18 * 0x20));
	BoneMatrix = mem->RPM<matrix3x4_t>(Bones + (Bone * sizeof(matrix3x4_t)));
	BoneOff = {BoneMatrix.m_flMatVal[0][3], BoneMatrix.m_flMatVal[1][3], BoneMatrix.m_flMatVal[2][3]};

	DrawPos.ScreenRightlegBone = origin_v + BoneOff;

	return DrawPos;
}