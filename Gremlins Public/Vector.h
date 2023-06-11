#pragma once
#include <d3dx9math.h>
#include <cmath>

#define M_PI	3.14159265358979323846264338327950288419716939937510

constexpr float _Rad_To_Deg = 57.2957795131f;
constexpr float _Deg_To_Rad = 0.01745329251f;

class Vector2
{
public:
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
};

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3();
	Vector3(float x, float y, float z);

	Vector3 operator+(const Vector3& vector) const;
	Vector3 operator-(const Vector3& vector) const;
	Vector3 operator-() const;
	Vector3 operator*(float number) const;
	Vector3 operator/(float number) const;

	Vector3& operator+=(const Vector3& vector);
	Vector3& operator-=(const Vector3& vector);
	Vector3& operator*=(float number);
	Vector3& operator/=(float number);

	void normalize();

	bool operator==(const Vector3& vector) const;
	bool operator!=(const Vector3& vector) const;

	inline float _Length(Vector3 vect)
	{
		return sqrt(vect.x * vect.x + vect.y * vect.y + vect.z * vect.z);
	}

	inline float _Dot(const Vector3& vector)
	{
		return x * vector.x + y * vector.y + z * vector.z;
	}

	float Distance(const Vector3& vector)
	{
		return float(sqrtf(powf(vector.x - x, 2.0) + powf(vector.y - y, 2.0) + powf(vector.z - z, 2.0)));
	}
};

inline bool Vector3::operator==(const Vector3& vector) const
{
	return x == vector.x && y == vector.y && z == vector.z;
}

inline bool Vector3::operator!=(const Vector3& vector) const
{
	return x != vector.x || y != vector.y || z != vector.z;
}

inline Vector3 Vector3::operator+(const Vector3& vector) const
{
	return Vector3(x + vector.x, y + vector.y, z + vector.z);
}

inline Vector3 Vector3::operator-(const Vector3& vector) const
{
	return Vector3(x - vector.x, y - vector.y, z - vector.z);
}

inline Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator*(float number) const
{
	return Vector3(x * number, y * number, z * number);
}

inline Vector3 Vector3::operator/(float number) const
{
	return Vector3(x / number, y / number, z / number);
}

// Vector4
class Vector4
{
public:
	float w;
	float x;
	float y;
	float z;

	Vector4();
	Vector4(float w, float x, float y, float z);

	Vector4 operator+(const Vector4& vector) const;
	Vector4 operator-(const Vector4& vector) const;
	Vector4 operator-() const;
	Vector4 operator*(float number) const;
	Vector4 operator/(float number) const;

	Vector4& operator+=(const Vector4& vector);
	Vector4& operator-=(const Vector4& vector);
	Vector4& operator*=(float number);
	Vector4& operator/=(float number);

	bool operator==(const Vector4& vector) const;
	bool operator!=(const Vector4& vector) const;

	inline float Dot(const Vector4& vector)
	{
		return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
	}

	inline float Distance(const Vector4& vector)
	{
		return sqrt(
			(x - vector.x) * (x - vector.x) +
			(y - vector.y) * (y - vector.y) +
			(z - vector.z) * (z - vector.z) +
			(w - vector.w) * (w - vector.w));
	}
};

inline bool Vector4::operator==(const Vector4& vector) const
{
	return x == vector.x && y == vector.y && z == vector.z && w == vector.w;
}

inline bool Vector4::operator!=(const Vector4& vector) const
{
	return x != vector.x || y != vector.y || z != vector.z || w != vector.w;
}

inline Vector4 Vector4::operator+(const Vector4& vector) const
{
	return Vector4(x + vector.x, y + vector.y, z + vector.z, w + vector.w);
}

inline Vector4 Vector4::operator-(const Vector4& vector) const
{
	return Vector4(x - vector.x, y - vector.y, z - vector.z, w - vector.w);
}

inline Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator*(float number) const
{
	return Vector4(x * number, y * number, z * number, w * number);
}

inline Vector4 Vector4::operator/(float number) const
{
	return Vector4(x / number, y / number, z / number, w / number);
}

class Vector6
{
public:
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;

	Vector6();
	Vector6(float a, float b, float c, float d, float e, float f);
};

struct MatrixA
{
	float matrix[16];
};


#define Assert( _exp ) ((void)0)
struct matrix3x4_t
{
	matrix3x4_t() {}
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	float* operator[](int i) { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	const float* operator[](int i) const { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	float* Base() { return &m_flMatVal[0][0]; }
	const float* Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};