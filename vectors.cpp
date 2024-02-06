#include "vectors.h"

V2 v2(float arg)
{
	V2 result;
	result.m_x = arg;
	result.m_y = arg;
	return result;
}

V2 v2(float x, float y)
{
	V2 result;
	result.m_x = x;
	result.m_y = y;
	return result;
}

V2 v2(int x, int y)
{
	V2 result;
	result.m_x = x;
	result.m_y = y;
	return result;
}

V2 v2(unsigned int x, unsigned int y)
{
	V2 result;
	result.m_x = (float)x;
	result.m_y = (float)y;
	return result;
}

V2 operator+(const V2 &a, const V2 &b)
{
	V2 result;
	result.m_x = a.m_x + b.m_x;
	result.m_y = a.m_y + b.m_y;
	return result;
}

V2 operator-(const V2 &a, const V2 &b)
{
	V2 result;
	result.m_x = a.m_x - b.m_x;
	result.m_y = a.m_y - b.m_y;
	return result;
}

V2 operator*(float a, const V2 &b)
{
	V2 result;
	result.m_x = a * b.m_x;
	result.m_y = a * b.m_y;
	return result;
}

V2 operator*(const V2 &a, const V2 &b)
{
	V2 result;
	result.m_x = a.m_x * b.m_x;
	result.m_y = a.m_y * b.m_y;
	return result;
}

V2 operator/(const V2 &a, float b)
{
	V2 result;
	result.m_x = a.m_x / b;
	result.m_y = a.m_y / b;
	return result;
}

V3 v3(float x, float y, float z)
{
	V3 result;
	result.m_x = x;
	result.m_y = y;
	result.m_z = z;
	return result;
}

V3 operator+(const V3 &a, const V3 &b)
{
	V3 result;
	result.m_x = a.m_x + b.m_x;
	result.m_y = a.m_y + b.m_y;
	result.m_z = a.m_z + b.m_z;
	return result;
}

V3 operator-(const V3 &a, const V3 &b)
{
	V3 result;
	result.m_x = a.m_x - b.m_x;
	result.m_y = a.m_y - b.m_y;
	result.m_z = a.m_z - b.m_z;
	return result;
}

V3 operator+=(V3 &a, const V3 &b)
{
	a = a + b;
	return a;
}

V3 operator-=(V3 &a, const V3 &b)
{
	a = a - b;
	return a;
}

V3 operator*(const V3 &a, float b)
{
	V3 result;
	result.m_x = a.m_x * b;
	result.m_y = a.m_y * b;
	result.m_z = a.m_z * b;
	return result;
}

V3 operator*(float a, const V3 &b)
{
	V3 result;
	result.m_x = b.m_x * a;
	result.m_y = b.m_y * a;
	result.m_z = b.m_z * a;
	return result;
}


V3 operator-(V3 &a)
{
	V3 result;
	result.m_x = -a.m_x;
	result.m_y = -a.m_y;
	result.m_z = -a.m_z;
	return result;
}

V4 v4(const V3 &a, float w)
{
	V4 result{};
	result.m_xyz = a;
	result.m_alpha = w;
	return result;
}

V4 operator+(const V4 &a, const V4 &b)
{
	V4 result{};
	result.m_x = a.m_x + b.m_x;
	result.m_y = a.m_y + b.m_y;
	result.m_z = a.m_z + b.m_z;
	result.m_w = a.m_w + b.m_w;

	return result;
}

V4 operator*(const V4 &a, float b)
{
	V4 result{};
	result.m_x = a.m_x * b;
	result.m_y = a.m_y * b;
	result.m_z = a.m_z * b;
	result.m_w = a.m_w * b;

	return result;
}

V4 operator*(const M4 &a, const V4 &b)
{
	V4 result = a.m_v[0] * b.m_x + a.m_v[1] * b.m_y + a.m_v[2] * b.m_z + a.m_v[3] * b.m_w;
	return result;
}