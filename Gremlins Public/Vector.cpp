#include "Vector.h"

Vector2::Vector2()
{
}

Vector2::Vector2(float x, float y)
	: x(x), y(y)
{
}

Vector3::Vector3()
{
}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

Vector3& Vector3::operator+=(const Vector3& vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;

	return *this;
}

Vector3& Vector3::operator*=(float number)
{
	x *= number;
	y *= number;
	z *= number;

	return *this;
}

Vector3& Vector3::operator/=(float number)
{
	x /= number;
	y /= number;
	z /= number;

	return *this;
}

float normalized_angle(float angle) {
	const auto normalize_angle = [](float& angle) {
		if (!std::isfinite(angle)) {
			angle = 0.f;
			return;
		}

		if (angle >= -180.f && angle <= 180.f)
			return;

		const auto rot = std::round(std::abs(angle / 360.f));
		angle = angle < 0.f ? angle + 360.f * rot : angle - 360.f * rot;
	};

	normalize_angle(angle);
	return angle;
}

void Vector3::normalize() {
	x = normalized_angle(x);
	y = normalized_angle(y);
	z = normalized_angle(z);
}

// Vector 4
Vector4::Vector4()
{
}

Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{
}

Vector4& Vector4::operator+=(const Vector4& vector)
{
	x += vector.x;
	y += vector.y;
	z += vector.z;
	w += vector.w;

	return *this;
}

Vector4& Vector4::operator-=(const Vector4& vector)
{
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;
	w -= vector.w;

	return *this;
}

Vector4& Vector4::operator*=(float number)
{
	x *= number;
	y *= number;
	z *= number;
	w *= number;

	return *this;
}

Vector4& Vector4::operator/=(float number)
{
	x /= number;
	y /= number;
	z /= number;
	w /= number;

	return *this;
}

Vector6::Vector6()
{

}

Vector6::Vector6(float a, float b, float c, float d, float e, float f)
	: a(a), b(b), c(c), d(d), e(e), f(f)
{

}