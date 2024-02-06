#pragma once

union M4
{
	V4 m_v[4];
	float m_e[16];
};

M4 identityM4();
M4 operator*(const M4 &a, const M4 &b);
M4 scaleMatrix(float x, float y, float z);
M4 translationMatrix(float x, float y, float z);
M4 rotationMatrix(float x, float y, float z);
M4 translationMatrix(const V3 &pos);
