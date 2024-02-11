#include <cmath>
#include <array>

#include "matrices.hpp"

M4 identityM4()
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

M4 scaleMatrix(float x, float y, float z)
{
	M4 result = identityM4();
	result.m_v[0].m_x = x;
	result.m_v[1].m_y = y;
	result.m_v[2].m_z = z;
	return result;
}

M4 translationMatrix(float x, float y, float z)
{
	M4 result = identityM4();
	result.m_v[3].m_xyz = v3(x, y, z);
	return result;
}

M4 translationMatrix(const V3 &pos)
{
	M4 result = translationMatrix(pos.m_x, pos.m_y, pos.m_z);
	return result;
}

M4 rotationMatrix(float x, float y, float z)
{
	M4 result{};

	M4 rotateX = identityM4();
	rotateX.m_v[1].m_y = cos(x);
	rotateX.m_v[2].m_y = -sin(x);
	rotateX.m_v[1].m_z = sin(x);
	rotateX.m_v[2].m_z = cos(x);

	M4 rotateY = identityM4();
	rotateY.m_v[0].m_x = cos(y);
	rotateY.m_v[2].m_x = sin(y);
	rotateY.m_v[0].m_z = -sin(y);
	rotateY.m_v[2].m_z = cos(y);

	M4 rotateZ = identityM4();
	rotateZ.m_v[0].m_x = cos(z);
	rotateZ.m_v[1].m_x = -sin(z);
	rotateZ.m_v[0].m_y = sin(z);
	rotateZ.m_v[1].m_y = cos(z);

	result = rotateZ * rotateY * rotateX;

	return result;
}

V4 operator*(const M4 &a, const V4 &b)
{
	V4 result = a.m_v[0] * b.m_x + a.m_v[1] * b.m_y + a.m_v[2] * b.m_z + a.m_v[3] * b.m_w;
	return result;
}