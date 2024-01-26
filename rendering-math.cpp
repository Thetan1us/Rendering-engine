// V2
#include "rendering-engine.h"
#include "rendering-math.h"

V2 v2(f32 arg)
{
	V2 result;
	result.m_x = arg;
	result.m_y = arg;
	return result;
}

V2 v2(f32 x, f32 y)
{
	V2 result;
	result.m_x = x;
	result.m_y = y;
	return result;
}

V2 v2(i32 x, i32 y)
{
	V2 result;
	result.m_x = x;
	result.m_y = y;
	return result;
}

V2 v2(u32 x, u32 y)
{
	V2 result;
	result.m_x = (f32)x;
	result.m_y = (f32)y;
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

V2 operator*(f32 a, const V2 &b)
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

V2 operator/(const V2 &a, f32 b)
{
	V2 result;
	result.m_x = a.m_x / b;
	result.m_y = a.m_y / b;
	return result;
}

V3 v3(f32 x, f32 y, f32 z)
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

V3 operator*(const V3 &a, f32 b)
{
	V3 result;
	result.m_x = a.m_x * b;
	result.m_y = a.m_y * b;
	result.m_z = a.m_z * b;
	return result;
}

V3 operator*(f32 a, const V3 &b)
{
	V3 result;
	result.m_x = b.m_x * a;
	result.m_y = b.m_y * a;
	result.m_z = b.m_z * a;
	return result;
}

V4 operator+(const V4 &a, const V4 &b)
{
	V4 result{};
	result.m_x = a.m_x + b.m_x;
	result.m_y = a.m_y + b.m_y;
	result.m_z = a.m_z + b.m_z;

	return result;
}

V4 operator*(const V4 &a, f32 b)
{
	V4 result{};
	result.m_x = a.m_x * b;
	result.m_y = a.m_y * b;
	result.m_z = a.m_z * b;

	return result;
}

V4 operator*(const M4 &a, const V4 &b)
{
	V4 result = a.m_v[0] * b.m_x + a.m_v[1] * b.m_y + a.m_v[2] * b.m_z + a.m_v[3] * b.m_w;
	return result;
}

M4 createM4()
{
	M4 result{};
	result.m_v[0].m_x = 1.0f;
	result.m_v[1].m_y = 1.0f;
	result.m_v[2].m_z = 1.0f;
	result.m_v[3].m_w = 1.0f;
	return result;
}

M4 operator*(const M4 &a, const M4 &b)
{
	M4 result{};
	result.m_v[0] = a * b.m_v[0];
	result.m_v[1] = a * b.m_v[1];
	result.m_v[2] = a * b.m_v[2];
	result.m_v[3] = a * b.m_v[3];
	return result;
}

M4 scaleMatrix(f32 x, f32 y, f32 z)
{
	M4 result = createM4();
	result.m_v[0].m_x = x;
	result.m_v[1].m_y = y;
	result.m_v[2].m_z = z;
	return result;
}

M4 translationMatrix(f32 x, f32 y, f32 z)
{
	M4 result = createM4();
	result.m_v[3].m_xyz = v3(x, y, z);
	return result;
}

M4 rotationMatrix(f32 x, f32 y, f32 z)
{
	M4 result{};

	M4 rotateX = createM4();
	rotateX.m_v[1].m_y = cos(x);
	rotateX.m_v[2].m_z = -sin(x);
	rotateX.m_v[1].m_z = sin(x);
	rotateX.m_v[2].m_z = cos(x);

	M4 rotateY = createM4();
	rotateY.m_v[0].m_x = cos(y);
	rotateY.m_v[2].m_z = -sin(y);
	rotateY.m_v[0].m_z = sin(y);
	rotateY.m_v[2].m_z = cos(y);

	M4 rotateZ = createM4();
	rotateZ.m_v[0].m_x = cos(z);
	rotateZ.m_v[1].m_z = -sin(z);
	rotateZ.m_v[0].m_x = sin(z);
	rotateZ.m_v[1].m_y = cos(z);

	result = rotateZ * rotateY * rotateX;

	return result;
}
