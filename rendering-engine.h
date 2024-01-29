#pragma once

#include <cstdint>
#include <cstddef>
#include <cfloat>
#include <iostream>
#include <vector>
#include <array>

#include <Windows.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define Assert(Expression) if (!(Expression)) { DebugBreak(); }
#define InvalidCodePath Assert(!"Invalid Code Path")

#define KiloBytes(Val) ((Val) * 1024LL)
#define MegaBytes(Val) (KiloBytes(Val) * 1024LL)
#define GigaBytes(Val) (MegaBytes(Val) * 1024LL)
#define TeraBytes(Val) (GigaBytes(Val) * 1024LL)


struct GlobalState
{
	HWND windowHandle;
	bool isRunning;
	HDC deviceContext;
	u32 frameBufferWidth;
	u32 frameBufferHeight;
	std::vector<u32> frameBufferPixels;
	std::vector<f32> depthBuffer;

	f32 currentTime;
};