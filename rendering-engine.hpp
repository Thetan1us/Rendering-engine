#pragma once

#include <cstddef>
#include <cfloat>
#include <iostream>

#define Assert(Expression) if (!(Expression)) { DebugBreak(); }
#define InvalidCodePath Assert(!"Invalid Code Path")

#define KiloBytes(Val) ((Val) * 1024LL)
#define MegaBytes(Val) (KiloBytes(Val) * 1024LL)
#define GigaBytes(Val) (MegaBytes(Val) * 1024LL)
#define TeraBytes(Val) (GigaBytes(Val) * 1024LL)

struct Texture
{
	uint32_t m_width;
	uint32_t m_height;
	std::vector<uint32_t> m_textels;
};

enum SamplerType
{
	SamplerType_none,
	SamplerType_nearest,
	SamplerType_bilinear,
};

struct Sampler
{
	SamplerType m_type;
	uint32_t m_borderColor;
};

struct Camera
{
	bool m_prevMousedown;
	V2 m_prevMousePos;
	V3 m_pos;

	float m_yaw;
	float m_pitch;
	
};

struct GlobalState
{
	HWND windowHandle;
	bool isRunning;
	HDC deviceContext;
	uint32_t frameBufferWidth;
	uint32_t frameBufferHeight;
	std::vector<uint32_t> frameBufferPixels;
	std::vector<float> depthBuffer;

	float currentTime;

	bool wDown;
	bool aDown;
	bool sDown;
	bool dDown;
	bool qDown;
	bool eDown;

	Camera camera;
};