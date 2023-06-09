#include "interface.h"

int utils::GetUnixTimestamp()
{
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	std::chrono::system_clock::duration dtn = tp.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::seconds>(dtn).count();
}

bool utils::WorldToScreen(MatrixA viewMatrix, Vector3 world, Vector3& screen)
{
	float* m_vMatrix = viewMatrix.matrix;
	float w = m_vMatrix[12] * world.x + m_vMatrix[13] * world.y + m_vMatrix[14] * world.z + m_vMatrix[15];

	screen.x = m_vMatrix[0] * world.x + m_vMatrix[1] * world.y + m_vMatrix[2] * world.z + m_vMatrix[3];
	screen.y = m_vMatrix[4] * world.x + m_vMatrix[5] * world.y + m_vMatrix[6] * world.z + m_vMatrix[7];

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	float Width = GetSystemMetrics(SM_CXSCREEN);
	float Height = GetSystemMetrics(SM_CYSCREEN);

	float x = Width / 2;
	float y = Height / 2;

	x += 0.5 * screen.x * Width + 0.5;
	y -= 0.5 * screen.y * Height + 0.5;

	screen.x = x;
	screen.y = y;

	if (w < 0.01f) return false;

	return true;
}

MatrixA utils::Get_ViewMatrix()
{
	uintptr_t viewRenderer = mem->RPM<uintptr_t>(BaseAddress + offset::viewrender);
	return  mem->RPM<MatrixA>(mem->RPM<uintptr_t>(viewRenderer + offset::viewmatrix));
}

void VAR_VectorAngless(const float* forward, float* angles)
{
	float	tmp, yaw, pitch;
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 57.295779513082f);
		if (yaw < 0)
			yaw += 360;
		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 57.295779513082f);
		if (pitch < 0)
			pitch += 360;
	}
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float VAR_Magnitudes(Vector3 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

Vector3 utils::VAR_Subtracts(Vector3 src, Vector3 dst)
{
	Vector3 diff;
	diff.x = src.x - dst.x;
	diff.y = src.y - dst.y;
	diff.z = src.z - dst.z;
	return diff;
}

Vector3 utils::VAR_toAngless(Vector3 const& v)
{
	double R2D = 57.2957795130823;
	Vector3 angles;
	angles.z = 0.0;
	angles.x = R2D * asin(v.z);
	angles.y = R2D * atan2(v.y, v.x);
	return angles;
}

Vector3 utils::VAR_toVectors(Vector3 const& angles)
{
	double D2R = 0.01745329251994334;
	Vector3 v;
	v.x = cos(D2R * angles.y) * cos(D2R * angles.x);
	v.y = sin(D2R * angles.y) * cos(D2R * angles.x);
	v.z = sin(D2R * angles.x);
	return v;
}

double utils::VAR_dots(Vector3 const& v0, Vector3 const& v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}


Vector3 utils::VAR_Adds(Vector3 src, Vector3 dst)
{
	Vector3 diff;
	diff.x = src.x + dst.x;
	diff.y = src.y + dst.y;
	diff.z = src.z + dst.z;
	return diff;
}

Vector3 utils::VAR_scales(float f, Vector3 const& v)
{
	Vector3 ret;
	ret.x = f * v.x;
	ret.y = f * v.y;
	ret.z = f * v.z;
	return ret;
}

Vector3 utils::VAR_units(Vector3 const& v)
{
	Vector3 result;
	double norm = sqrt(VAR_dots(v, v));
	if (norm < 1E-6)
	{
		result.x = 0.0;
		result.y = 0.0;
		result.z = 0.0;
	}
	else
	{
		float inv = (float)(1.0 / norm);
		result.x = inv * v.x;
		result.y = inv * v.y;
		result.z = inv * v.z;
	}
	return result;
}

Vector3 utils::Var_CalcAngless(Vector3& src, Vector3& dst)
{
	float output[3];
	float input[3] = { dst.x - src.x , dst.y - src.y, dst.z - src.z };
	VAR_VectorAngless(input, output);
	if (output[1] > 180) output[1] -= 360;
	if (output[1] < -180) output[1] += 360;
	if (output[0] > 180) output[0] -= 360;
	if (output[0] < -180) output[0] += 360;
	return { output[0], output[1], 0.f };
}

float utils::VAR_deltaDistances(Vector3 src, Vector3 dst)
{
	Vector3 diff = VAR_Subtracts(src, dst);
	return VAR_Magnitudes(diff);
}

float utils::biggestAngleChanges(Vector3 src, Vector3 dst)
{
	Vector3 diff;
	diff.x = abs(src.x - dst.x);
	diff.y = abs(src.y - dst.y);

	if (diff.x > 180) {
		diff.x = 360 - diff.x;
	}
	if (diff.y > 180) {
		diff.y = 360 - diff.y;
	}
	if (diff.x > diff.y)
	{
		return diff.x;
	}
	return diff.y;
}

float FastSQRT(float x) {
	union { int i; float x; } u;
	u.x = x; u.i = (u.i >> 1) + 0x1FC00000;
	u.x = u.x + x / u.x;
	return .25f * u.x + x / u.x;
}

float FastDist2D(const Vector3& Src, const Vector3& Dst) {
	return FastSQRT(powf(Src.x - Dst.x, 2.f) + powf(Src.y - Dst.y, 2.f));
}

float utils::GetFov(const Vector3& ScrCenter, const Vector3& ScreenPos)
{
	return FastDist2D(ScrCenter, ScreenPos);
}

float utils::GetHealthPercent(int Health) {
	float total = Health / 100.f;
	//std::cout << es("entity health : ") << entity.Health << std::endl;
	if (total > 1)
		return 1.0;
	if (total < .01)
		return 0.0;
	return total;
}

float utils::GetShieldPercent(int Shield) {
	float total = Shield / 100;
	if (total > 1)
		return 1.0;
	if (total < .01)
		return 0.0;
	return total;
}

Vector3 utils::Subtract(Vector3 src, Vector3 dst)
{
	Vector3 diff;
	diff.x = src.x - dst.x;
	diff.y = src.y - dst.y;
	diff.z = src.z - dst.z;
	return diff;
}

float utils::Magnitude(Vector3 vec)
{
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float utils::DeltaDistance(Vector3 src, Vector3 dst)
{
	Vector3 diff = Subtract(src, dst);
	return Magnitude(diff);
}