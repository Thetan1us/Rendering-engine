#pragma once

#include "vectors.hpp"

union M4
{
	std::array<V4, 4> m_v;
	std::array<float, 16> m_e;
};

M4 identityM4();
M4 operator*(const M4 &a, const M4 &b);
M4 scaleMatrix(float x, float y, float z);
M4 translationMatrix(float x, float y, float z);
M4 translationMatrix(const V3 &pos);
M4 rotationMatrix(float x, float y, float z);
V4 operator*(const M4 &a, const V4 &b);
M4 perspectiveMatrix(float fov, float aspectRatio, float nearZ, float farZ);