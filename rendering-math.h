#pragma once 

static f32 Pi32 = 3.14159f;

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
		f32 m_red, m_green, m_blue;
	};

	struct
	{
		V2 m_xy;
		f32 m_ignored;
	};

	f32 m_e[3];
};

union V4
{
	struct
	{
		f32 m_x, m_y, m_z, m_w;
	};

	struct
	{
		f32 m_red, m_green, m_blue, m_alpha;
	};

	struct
	{
		V3 m_xyz;
		f32 m_ignored0;
	};

	struct
	{
		V2 m_xy;
		V2 m_ignored1;
	};

	f32 m_e[4];
};

union M4
{
	V4 m_v[4];
	f32 m_e[16];
};

V2 v2(f32 arg);
V2 v2(f32 x, f32 y);
V2 v2(i32 x, i32 y);
V2 v2(u32 x, u32 y);
V2 operator+(const V2 &a, const V2 &b);
V2 operator-(const V2 &a, const V2 &b);
V2 operator*(f32 a, const V2 &b);
V2 operator*(const V2 &a, const V2 &b);
V2 operator/(const V2 &a, f32 b);
V3 v3(f32 x, f32 y, f32 z);
V3 operator+(const V3 &a, const V3 &b);
V3 operator*(const V3 &a, f32 b);
V3 operator*(f32 a, const V3 &b);
V4 v4(const V3 &a, f32 w);
V4 operator+(const V4 &a, const V4 &b);
V4 operator*(const V4 &a, f32 b);
V4 operator*(const M4 &a, const V4 &b);
M4 identityM4();
M4 operator*(const M4 &a, const M4 &b);
M4 scaleMatrix(f32 x, f32 y, f32 z);
M4 translationMatrix(f32 x, f32 y, f32 z);
M4 rotationMatrix(f32 x, f32 y, f32 z);