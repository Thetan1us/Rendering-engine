#include <cmath>

#include "rendering-engine.h"
#include "rendering-math.h"

static GlobalState globalState;

V2 projectPoint(const V3 &pos)
{
	// NDC
	V2 result{};
	result = pos.m_xy / pos.m_z;
	// Pixels
	result = 0.5f * (result + v2(1.0f, 1.0f));
	result = result * v2(globalState.frameBufferWidth, globalState.frameBufferHeight);
	return result;
}

f32 vectorProduct(const V2 &a, const V2 &b)
{
	f32 result = a.m_x * b.m_y - a.m_y * b.m_x;
	return result;
}

void drawTriangle(V3 *points, V3 *colors)
{
	V2 pointA = projectPoint(points[0]);
	V2 pointB = projectPoint(points[1]);
	V2 pointC = projectPoint(points[2]);

	i32 edgePointLeft = min((i32)pointA.m_x, min((i32)pointB.m_x, (i32)pointC.m_x));
	i32 edgePointRight = max((i32)ceil(pointA.m_x), max((i32)ceil(pointB.m_x), (i32)ceil(pointC.m_x)));
	i32 edgePointBottom = min((i32)pointA.m_y, min((i32)pointB.m_y, (i32)pointC.m_y));
	i32 edgePointTop = max((i32)ceil(pointA.m_y), max((i32)ceil(pointB.m_y), (i32)ceil(pointC.m_y)));

	edgePointLeft = max(0, edgePointLeft);
	edgePointLeft = min(globalState.frameBufferWidth - 1, edgePointLeft);
	edgePointRight = max(0, edgePointRight);
	edgePointRight = min(globalState.frameBufferWidth - 1, edgePointRight);
	edgePointTop = max(0, edgePointTop);
	edgePointTop = min(globalState.frameBufferHeight - 1, edgePointTop);
	edgePointBottom = max(0, edgePointBottom);
	edgePointBottom = min(globalState.frameBufferHeight - 1, edgePointBottom);


	V2 triangleEdge0 = pointB - pointA;
	V2 triangleEdge1 = pointC - pointB;
	V2 triangleEdge2 = pointA - pointC;

	bool isTopLeft0 = (triangleEdge0.m_x >= 0.0f && triangleEdge0.m_y > 0.0f) || (triangleEdge0.m_x > 0 && triangleEdge0.m_y == 0.0f);
	bool isTopLeft1 = (triangleEdge1.m_x >= 0.0f && triangleEdge1.m_y > 0.0f) || (triangleEdge1.m_x > 0 && triangleEdge1.m_y == 0.0f);
	bool isTopLeft2 = (triangleEdge2.m_x >= 0.0f && triangleEdge2.m_y > 0.0f) || (triangleEdge2.m_x > 0 && triangleEdge2.m_y == 0.0f);

	f32 barycentricDiv = vectorProduct(pointB - pointA, pointC - pointA);

	for (i32 y = edgePointTop; y >= edgePointBottom; --y)
	{
		for (i32 x = edgePointLeft; x <= edgePointRight; ++x)
		{
			V2 pixelPoint = v2(x, y) + v2(0.5f, 0.5f);

			V2 centerPixelVect0 = pixelPoint - pointA;
			V2 centerPixelVect1 = pixelPoint - pointB;
			V2 centerPixelVect2 = pixelPoint - pointC;

			f32 vectorLength0 = vectorProduct(centerPixelVect0, triangleEdge0);
			f32 vectorLength1 = vectorProduct(centerPixelVect1, triangleEdge1);
			f32 vectorLength2 = vectorProduct(centerPixelVect2, triangleEdge2);

			if ((vectorLength0 > 0.0f || (isTopLeft0 && vectorLength0 == 0.0f)) &&
				(vectorLength1 > 0.0f || (isTopLeft1 && vectorLength1 == 0.0f)) &&
				(vectorLength2 > 0.0f || (isTopLeft2 && vectorLength2 == 0.0f)))
			{
				u32 pixelID = y * globalState.frameBufferWidth + x;

				f32 t0 = -vectorLength1 / barycentricDiv;
				f32 t1 = -vectorLength2 / barycentricDiv;
				f32 t2 = -vectorLength0 / barycentricDiv;
				V3 finalColor = t0 * colors[0] + t1 * colors[1] + t2 * colors[2];
				finalColor = finalColor * 255.0f;
				u32 finalColorU32 = (u32)0xFF << 24 | (u32)finalColor.m_red << 16 | (u32)finalColor.m_green << 8 | (u32)finalColor.m_blue;

				globalState.frameBufferPixels[pixelID] = finalColorU32;
			}
		}
	}
}

LRESULT Win32WindowCallback(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	LRESULT result = {};

	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	{
		globalState.isRunning = false;
	} break;
	default:
	{
		result = DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}
	}
	return result;
}

int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd)
{
	globalState.isRunning = true;
	LARGE_INTEGER timerFrequency{};
	Assert(QueryPerformanceFrequency(&timerFrequency));

	{
		WNDCLASSA windowClass{};
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = Win32WindowCallback;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursorA(NULL, IDC_HAND);
		windowClass.lpszClassName = "Rendering Engine";

		if (!RegisterClassA(&windowClass))
			InvalidCodePath;

		globalState.windowHandle = CreateWindowExA(
			0,
			windowClass.lpszClassName,
			"Rendering Engine",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			1280,
			720,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if (!globalState.windowHandle)
			InvalidCodePath;

		globalState.deviceContext = GetDC(globalState.windowHandle);
	}

	{
		RECT clientRect{};
		GetClientRect(globalState.windowHandle, &clientRect);
		globalState.frameBufferWidth = clientRect.right - clientRect.left;
		globalState.frameBufferHeight = clientRect.bottom - clientRect.top;
		globalState.frameBufferPixels.resize(globalState.frameBufferHeight * globalState.frameBufferWidth);
	}

	LARGE_INTEGER startTime{};
	LARGE_INTEGER endTime{};
	Assert(QueryPerformanceCounter(&startTime));

	while (globalState.isRunning)
	{
		Assert(QueryPerformanceCounter(&endTime));
		f32 frameTime = f32(endTime.QuadPart - startTime.QuadPart) / f32(timerFrequency.QuadPart);
		startTime = endTime;

		MSG message{};
		while (PeekMessageA(&message, globalState.windowHandle, 0, 0, PM_REMOVE))
		{
			switch (message.message)
			{
			case WM_QUIT: globalState.isRunning = false; break;
			default:
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
				break;
			}
			}
		}

		// All pixels are set to 0

		for (u32 y = 0; y < globalState.frameBufferHeight; ++y)
		{
			for (u32 x = 0; x < globalState.frameBufferWidth; ++x)
			{
				u32 pixelID = y * globalState.frameBufferWidth + x;

				u8 alpha = 255;
				u8 red = (u8)0;
				u8 green = (u8)0;
				u8 blue = (u8)0;
				u32 pixelColor = (u32)alpha << 24 | (u32)red << 16 | (u32)green << 8 | (u32)blue;

				globalState.frameBufferPixels[pixelID] = pixelColor;
			}
		}

		globalState.currentAngle += frameTime;
		if (globalState.currentAngle >= 2.0f * Pi32)
			globalState.currentAngle = 0.0f;

		V3 colors[] =
		{
			V3{1,0,0},
			V3{0,1,0},
			V3{0,0,1},
		};

		for (int triangleID = 5; triangleID >= 0; --triangleID)
		{
			f32 depth = std::powf(2.0f, triangleID + 1.0f);
			V3 points[3] =
			{
				V3{ -10.0f, -0.5f, depth },
				V3{ 0.0f, 0.5f, depth },
				V3{ 1.0f, -0.5f, depth }
			};

			for (int pointID = 0; pointID < ArrayCount(points); ++pointID)
			{
				V3 transformedPoint = points[pointID] + v3(cos(globalState.currentAngle), sin(globalState.currentAngle), 0);
				points[pointID] = transformedPoint;
			}

			drawTriangle(points, colors);
		}

		RECT clientRect{};
		GetClientRect(globalState.windowHandle, &clientRect);
		u32 clientWidth = clientRect.right - clientRect.left;
		u32 clientHeight = clientRect.bottom - clientRect.top;

		BITMAPINFO bitMapInfo{};

		bitMapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		bitMapInfo.bmiHeader.biWidth = globalState.frameBufferWidth;
		bitMapInfo.bmiHeader.biHeight = globalState.frameBufferHeight;
		bitMapInfo.bmiHeader.biPlanes = 1;
		bitMapInfo.bmiHeader.biBitCount = 32;
		bitMapInfo.bmiHeader.biCompression = BI_RGB;
		bitMapInfo.bmiHeader.biSizeImage = 0;

		Assert(StretchDIBits(
			globalState.deviceContext,
			0,
			0,
			clientWidth,
			clientHeight,
			0,
			0,
			globalState.frameBufferWidth,
			globalState.frameBufferHeight,
			globalState.frameBufferPixels.data(),
			&bitMapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		));

	}

	return 0;
}