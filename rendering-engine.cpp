#include <cmath>

#include "rendering-engine.h"
#include "rendering-math.h"

static GlobalState globalState;

V2 projectPoint(V3 pos)
{
	// NDC
	V2 result{};
	result = pos.m_xy / pos.m_z;
	// Pixels
	result = 0.5f * (result + v2(1.0f));
	result = result * v2(globalState.frameBufferWidth, globalState.frameBufferHeight);
	return result;
}

f32 vectorProduct(V2 a, V2 b)
{
	f32 result = a.m_x * b.m_y - a.m_y * b.m_x;
	return result;
}

//For optimization

//std::vector<u32> findMinMaxPoints(const V2 &pointA, const V2 &pointB, const V2 &pointC)
//{
//	std::vector<u32> result{};
//	u32 edgePointLeft = (std::fmin)((u32)pointA.m_x, (std::fmin)((u32)pointB.m_x, (u32)pointC.m_x));
//	u32 edgePointRight = (std::fmax)((u32)pointA.m_x, (std::fmax)((u32)pointB.m_x, (u32)pointC.m_x));
//	u32 edgePointBottom = (std::fmin)((u32)pointA.m_y, (std::fmin)((u32)pointB.m_y, (u32)pointC.m_y));
//	u32 edgePointTop = (std::fmax)((u32)pointA.m_y, (std::fmax)((u32)pointB.m_y, (u32)pointC.m_y));
//
//	result.push_back(edgePointLeft);
//	result.push_back(edgePointRight);
//	result.push_back(edgePointBottom);
//	result.push_back(edgePointTop);
//
//	return result;
//}

void drawTriangle(V3 *points, u32 color)
{
	V2 pointA = projectPoint(points[0]);
	V2 pointB = projectPoint(points[1]);
	V2 pointC = projectPoint(points[2]);

	V2 triangleEdge0 = pointB - pointA;
	V2 triangleEdge1 = pointC - pointB;
	V2 triangleEdge2 = pointA - pointC;

	bool isTopLeft0 = (triangleEdge0.m_x >= 0.0f && triangleEdge0.m_y > 0.0f) || (triangleEdge0.m_x > 0 && triangleEdge0.m_y == 0.0f);
	bool isTopLeft1 = (triangleEdge1.m_x >= 0.0f && triangleEdge1.m_y > 0.0f) || (triangleEdge1.m_x > 0 && triangleEdge1.m_y == 0.0f);
	bool isTopLeft2 = (triangleEdge2.m_x >= 0.0f && triangleEdge2.m_y > 0.0f) || (triangleEdge2.m_x > 0 && triangleEdge2.m_y == 0.0f);

	//std::vector<u32> minMaxPoints = findMinMaxPoints(pointA, pointB, pointC);

	for (u32 y = 0; y < globalState.frameBufferHeight; ++y)
		//for (u32 y = minMaxPoints[3]; y < minMaxPoints[2]; ++y)
	{
		for (u32 x = 0; x < globalState.frameBufferWidth; ++x)
			//for (u32 x = minMaxPoints[0]; x < minMaxPoints[1]; ++x)
		{
			V2 pixelPoint = v2(x, y) + v2(0.5f, 0.5f);

			V2 centerPixelVect0 = pixelPoint - pointA;
			V2 centerPixelVect1 = pixelPoint - pointB;
			V2 centerPixelVect2 = pixelPoint - pointC;

			f32 crossLength0 = vectorProduct(centerPixelVect0, triangleEdge0);
			f32 crossLength1 = vectorProduct(centerPixelVect1, triangleEdge1);
			f32 crossLength2 = vectorProduct(centerPixelVect2, triangleEdge2);

			if ((crossLength0 > 0.0f || (isTopLeft0 && crossLength0 == 0.0f)) &&
				(crossLength1 > 0.0f || (isTopLeft1 && crossLength1 == 0.0f)) &&
				(crossLength2 > 0.0f || (isTopLeft2 && crossLength2 == 0.0f)))
			{
				u32 pixelID = y * globalState.frameBufferWidth + x;
				globalState.frameBufferPixels[pixelID] = color;
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

		globalState.frameBufferWidth = 50;
		globalState.frameBufferHeight = 50;

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

		u32 colors[] =
		{
			0xFFFF00FF,
			0xFFFF0000,
			0xFF00FF00,
			0xFF0000FF,
			0xFFFFFFFF,
		};

		V3 points1[3] =
		{
			v3(-1.0f, -1.0f, 1.0f),
			v3(-1.0f, 1.0f, 1.0f),
			v3(1.0f, 1.0f, 1.0f),
		};

		V3 points2[3] =
		{
			v3(-1.0f, -1.0f, 1.0f),
			v3(1.0f, 1.0f, 1.0f),
			v3(1.0f, -1.0f, 1.0f),
		};

		drawTriangle(points2, colors[2]);
		drawTriangle(points1, colors[0]);

#if 0
		for (int triangleID = 10; triangleID >= 0; --triangleID)
		{
			f32 depth = std::powf(2.0f, triangleID + 1.0f);
			V3 points[3] =
			{
				V3{ -0.9f, -0.5f, depth },
				V3{ 0.0f, 0.5f, depth },
				V3{ 1.0f, -0.5f, depth }
			};

			for (int pointID = 0; pointID < ArrayCount(points); ++pointID)
			{
				V3 transformedPoint = points[pointID] + v3(cos(globalState.currentAngle), sin(globalState.currentAngle), 0);
				points[pointID] = transformedPoint;
			}

			drawTriangle(points, colors[triangleID % ArrayCount(colors)]);
		}
#endif
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