#pragma once

union M4;

union V2
{
	struct
	{
		float m_x, m_y;
	};

	std::array<float, 2> m_e;
};

union V3
{
	struct
	{
		float m_x, m_y, m_z;
	};

	struct
	{
		float m_red, m_green, m_blue;
	};

	struct
	{
		V2 m_xy;
		float m_ignored;
	};

	std::array<float, 3> m_e;
};

union V4
{
	struct
	{
		float m_x, m_y, m_z, m_w;
	};

	struct
	{
		float m_red, m_green, m_blue, m_alpha;
	};

	struct
	{
		V3 m_xyz;
		float m_ignored0;
	};

	struct
	{
		V2 m_xy;
		V2 m_ignored1;
	};

	std::array<float, 4> m_e;
};

float vectorProduct(const V2 &a, const V2 &b);
V2 v2(float arg);
V2 v2(float x, float y);
V2 v2(int x, int y);
V2 v2(uint32_t x, uint32_t y);
V2 operator+(const V2 &a, const V2 &b);
V2 operator-(const V2 &a, const V2 &b);
V3 operator+=(V3 &a, const V3 &b);
V3 operator-=(V3 &a, const V3 &b);
V2 operator/=(V2 &a, const float b);
V2 operator*(float a, const V2 &b);
V2 operator*(const V2 &a, const V2 &b);
V2 operator/(const V2 &a, float b);
V3 v3(float x, float y, float z);
V3 operator+(const V3 &a, const V3 &b);
V3 operator*(const V3 &a, float b);
V3 operator*(float a, const V3 &b);
V3 operator-(const V3 &a);
V3 operator/=(V3 &a, const float b);
V3 normalize(const V3 &a);
V4 v4(const V3 &a, float w);
V4 v4(float x, float y, float z, float w);
V4 operator+(const V4 &a, const V4 &b);
V4 operator*(const V4 &a, float b);
