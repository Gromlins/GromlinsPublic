#include "aimbot.h"


Vector3 oldPunch{};
float old_lastCrosshair = -1.f;

Vector3 CalcAnglePredictives(Vector3 src, Vector3 dst, entity_t targetentity, entity_t local)
{
	if (!local.active_weapon_t.ptr)
		return {0,0,0};

	Vector3 Velocity = targetentity.abs_velocity_v;

	float bullet_vel = local.active_weapon_t.bullet_speed();
	float dist = utils::VAR_deltaDistances(src, dst);
	if (bullet_vel < 5000 || dist < 700) { // Not using gun or Super closeup
		return utils::Var_CalcAngless(src, dst);
	}

	float travel_time = dist / bullet_vel;
	float bullet_drop = (358.f * travel_time * travel_time);

	Vector3 predictedDst = dst;
	predictedDst.x += Velocity.x * travel_time;
	predictedDst.y += Velocity.y * travel_time;
	predictedDst.z += (Velocity.z * travel_time) + bullet_drop;

	return utils::Var_CalcAngless(src, predictedDst);
}

Vector3 SmoothAngleChanges(Vector3 src, Vector3 dst, float smoothFactor, float minAngle)
{
	if (minAngle > utils::biggestAngleChanges(src, dst)) { // Skip smooth if below minAngle
		return dst;
	}

	Vector3 v0 = utils::VAR_toVectors(src);
	Vector3 vf = utils::VAR_toVectors(dst);
	double dot_v0vf = utils::VAR_dots(v0, vf);
	double angle = acos(dot_v0vf) / smoothFactor; //desired rotation
	Vector3 n = utils::VAR_units(utils::VAR_Subtracts(vf, utils::VAR_scales(dot_v0vf, v0))); //projection
	Vector3 v = utils::VAR_Adds(utils::VAR_scales(cos(angle), v0), utils::VAR_scales(sin(angle), n)); //desired pointing

	return utils::VAR_toAngless(v);
}

Vector3 CalculateAnglesForEntss(entity_t local, entity_t targetEntity, int bone) {

	Vector3 newAngles = CalcAnglePredictives(local.cameraangle_v, targetEntity.get_bone_position(bone), targetEntity, local);

	Vector3 difff = local.dynamic_angles_v - local.view_angle_v;

	difff.z = 0;

	Vector3 angleCompensated = newAngles;

	angleCompensated.x -= difff.x;
	angleCompensated.y -= difff.y;

	return angleCompensated;
}

bool aimbot::is_in_crosshair(entity_t targetentity) {

	if (old_lastCrosshair == -1.f) {
		old_lastCrosshair = targetentity.crosshair_last_v;
		return false;
	}

	auto isCrosshair = targetentity.crosshair_last_v > old_lastCrosshair;
	old_lastCrosshair = targetentity.crosshair_last_v;

	return isCrosshair;
}

void aimbot::init(entity_t local, entity_t targetentity, int bone)
{
	if (targetentity.ptr == NULL) return;

	if (local.weapon2_v == -252) return;

	Vector3 newAngle = CalculateAnglesForEntss(local, targetentity, bone);

	if (newAngle.x != 0 && newAngle.y != 0)
	{
		if (setting::AimbotRcs)
		{
			newAngle = newAngle + (oldPunch - local.aim_punch_v) * (setting::rcsp / 100.f);
			//newAngle.normalize();
		}

		auto anglesmoothed = SmoothAngleChanges(local.view_angle_v, newAngle, setting::Smooth, 0.05);

		anglesmoothed.normalize();

		mem->WPM<Vector3>(local.ptr + offset::view_angle, anglesmoothed);

		oldPunch = local.aim_punch_v;
	}
}