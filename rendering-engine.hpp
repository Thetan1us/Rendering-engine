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

struct Camera
{
	V3 m_pos;
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