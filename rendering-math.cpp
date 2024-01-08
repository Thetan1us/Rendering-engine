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

V2 v2(u32 x, u32 y)
{
	V2 result;
	result.m_x = (f32)x;
	result.m_y = (f32)y;
	return result;
}

V2 operator+(V2 a, V2 b)
{
	V2 result;
	result.m_x = a.m_x + b.m_x;
	result.m_y = a.m_y + b.m_y;
	return result;
}

V2 operator-(V2 a, V2 b)
{
	V2 result;
	result.m_x = a.m_x - b.m_x;
	result.m_y = a.m_y - b.m_y;
	return result;
}

V2 operator*(f32 a, V2 b)
{
	V2 result;
	result.m_x = a * b.m_x;
	result.m_y = a * b.m_y;
	return result;
}

V2 operator*(V2 a, V2 b)
{
	V2 result;
	result.m_x = a.m_x * b.m_x;
	result.m_y = a.m_y * b.m_y;
	return result;
}

V2 operator/(V2 a, f32 b)
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

V3 operator+(V3 a, V3 b)
{
	V3 result;
	result.m_x = a.m_x + b.m_x;
	result.m_y = a.m_y + b.m_y;
	result.m_z = a.m_z + b.m_z;
	return result;
}