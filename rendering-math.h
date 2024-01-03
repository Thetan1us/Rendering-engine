#pragma once 

static f32 Pi32 = 3.14159265359f;

union V2
{
	struct
	{
		f32 m_x, m_y;
	};

	f32 m_e[2];
};

union V3
{
	struct
	{
		f32 m_x, m_y, m_z;
	};

	struct
	{
		V2 m_xy;
		f32 m_ignored;
	};

	f32 m_e[3];
};

V2 v2(f32 arg);
V2 v2(f32 x, f32 y);
V2 v2(u32 x, u32 y);
V2 operator+(V2 a, V2 b);
V2 operator*(f32 a, V2 b);
V2 operator*(V2 a, V2 b);
V2 operator/(V2 a, f32 b);
V3 v3(f32 x, f32 y, f32 z);
V3 operator+(V3 a, V3 b);