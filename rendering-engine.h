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

//struct Camera
//{
//	V3 m_pos;
//};

struct GlobalState
{
	HWND windowHandle;
	bool isRunning;
	HDC deviceContext;
	unsigned int frameBufferWidth;
	unsigned int frameBufferHeight;
	std::vector<unsigned int> frameBufferPixels;
	std::vector<float> depthBuffer;

	float currentTime;

	//Camera camera;
};