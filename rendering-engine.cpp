#include <vector>
#include <array>
#include <Windows.h>

#include "matrices.hpp"
#include "vectors.hpp"
#include "rendering-engine.hpp"

static GlobalState g_globalState;
static float g_Pi = 3.14159f;

V2 projectPoint(const V3 &m_pos)
{
	// NDC
	V2 result{};
	result = m_pos.m_xy / m_pos.m_z;
	// Pixels
	result = 0.5f * (result + v2(1.0f, 1.0f));
	result = result * v2(g_globalState.frameBufferWidth, g_globalState.frameBufferHeight);
	return result;
}

void drawTriangle(const V3 &modelVertex0, const V3 &modelVertex1, const V3 &modelVertex2,
	const V3 &modelColor0, const V3 &modelColor1, const V3 &modelColor2,
	const M4 &transform)
{
	V3 transformedPoint0 = (transform * v4(modelVertex0, 1.0f)).m_xyz;
	V3 transformedPoint1 = (transform * v4(modelVertex1, 1.0f)).m_xyz;
	V3 transformedPoint2 = (transform * v4(modelVertex2, 1.0f)).m_xyz;

	V2 pointA = projectPoint(transformedPoint0);
	V2 pointB = projectPoint(transformedPoint1);
	V2 pointC = projectPoint(transformedPoint2);

	int edgePointLeft = min((int)pointA.m_x, min((int)pointB.m_x, (int)pointC.m_x));
	int edgePointRight = max((int)ceil(pointA.m_x), max((int)ceil(pointB.m_x), (int)ceil(pointC.m_x)));
	int edgePointBottom = min((int)pointA.m_y, min((int)pointB.m_y, (int)pointC.m_y));
	int edgePointTop = max((int)ceil(pointA.m_y), max((int)ceil(pointB.m_y), (int)ceil(pointC.m_y)));

	edgePointLeft = max(0, edgePointLeft);
	edgePointLeft = min(g_globalState.frameBufferWidth - 1, edgePointLeft);
	edgePointRight = max(0, edgePointRight);
	edgePointRight = min(g_globalState.frameBufferWidth - 1, edgePointRight);
	edgePointTop = max(0, edgePointTop);
	edgePointTop = min(g_globalState.frameBufferHeight - 1, edgePointTop);
	edgePointBottom = max(0, edgePointBottom);
	edgePointBottom = min(g_globalState.frameBufferHeight - 1, edgePointBottom);

	V2 triangleEdge0 = pointB - pointA;
	V2 triangleEdge1 = pointC - pointB;
	V2 triangleEdge2 = pointA - pointC;

	bool isTopLeft0 = (triangleEdge0.m_x >= 0.0f && triangleEdge0.m_y > 0.0f) || (triangleEdge0.m_x > 0 && triangleEdge0.m_y == 0.0f);
	bool isTopLeft1 = (triangleEdge1.m_x >= 0.0f && triangleEdge1.m_y > 0.0f) || (triangleEdge1.m_x > 0 && triangleEdge1.m_y == 0.0f);
	bool isTopLeft2 = (triangleEdge2.m_x >= 0.0f && triangleEdge2.m_y > 0.0f) || (triangleEdge2.m_x > 0 && triangleEdge2.m_y == 0.0f);

	float barycentricDiv = vectorProduct(pointB - pointA, pointC - pointA);

	for (int y = edgePointTop; y >= edgePointBottom; --y)
	{
		for (int x = edgePointLeft; x <= edgePointRight; ++x)
		{
			V2 pixelPoint = v2(x, y) + v2(0.5f, 0.5f);

			V2 centerPixelVect0 = pixelPoint - pointA;
			V2 centerPixelVect1 = pixelPoint - pointB;
			V2 centerPixelVect2 = pixelPoint - pointC;

			float vectorLength0 = vectorProduct(centerPixelVect0, triangleEdge0);
			float vectorLength1 = vectorProduct(centerPixelVect1, triangleEdge1);
			float vectorLength2 = vectorProduct(centerPixelVect2, triangleEdge2);

			if ((vectorLength0 > 0.0f || (isTopLeft0 && vectorLength0 == 0.0f)) &&
				(vectorLength1 > 0.0f || (isTopLeft1 && vectorLength1 == 0.0f)) &&
				(vectorLength2 > 0.0f || (isTopLeft2 && vectorLength2 == 0.0f)))
			{
				uint32_t pixelID = y * g_globalState.frameBufferWidth + x;

				float t0 = -vectorLength1 / barycentricDiv;
				float t1 = -vectorLength2 / barycentricDiv;
				float t2 = -vectorLength0 / barycentricDiv;

				float depth = t0 * (1.0f / transformedPoint0.m_z) + t1 * (1.0f / transformedPoint1.m_z) + t2 * (1.0f / transformedPoint2.m_z);
				depth = 1.0f / depth;
				if (depth < g_globalState.depthBuffer[pixelID])
				{
					V3 finalColor = t0 * modelColor0 + t1 * modelColor1 + t2 * modelColor2;
					finalColor = finalColor * 255.0f;
					uint32_t finalColor32 = (uint32_t)0xFF << 24 | (uint32_t)finalColor.m_red << 16 | (uint32_t)finalColor.m_green << 8 | (uint32_t)finalColor.m_blue;

					g_globalState.frameBufferPixels[pixelID] = finalColor32;
					g_globalState.depthBuffer[pixelID] = depth;
				}

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
		g_globalState.isRunning = false;
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
	g_globalState.isRunning = true;
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

		g_globalState.windowHandle = CreateWindowExA(
			0,
			windowClass.lpszClassName,
			"Rendering Engine",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			740,
			700,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if (!g_globalState.windowHandle)
			InvalidCodePath;

		g_globalState.deviceContext = GetDC(g_globalState.windowHandle);
	}

	{
		RECT clientRect{};
		GetClientRect(g_globalState.windowHandle, &clientRect);
		g_globalState.frameBufferWidth = clientRect.right - clientRect.left;
		g_globalState.frameBufferHeight = clientRect.bottom - clientRect.top;

		g_globalState.frameBufferWidth = 500;
		g_globalState.frameBufferHeight = 500;

		g_globalState.frameBufferPixels.resize(g_globalState.frameBufferHeight * g_globalState.frameBufferWidth);
		g_globalState.depthBuffer.resize(g_globalState.frameBufferHeight * g_globalState.frameBufferWidth);
	}

	LARGE_INTEGER startTime{};
	LARGE_INTEGER endTime{};
	Assert(QueryPerformanceCounter(&startTime));

	while (g_globalState.isRunning)
	{
		Assert(QueryPerformanceCounter(&endTime));
		float frameTime = float(endTime.QuadPart - startTime.QuadPart) / float(timerFrequency.QuadPart);
		startTime = endTime;

		MSG message{};
		while (PeekMessageA(&message, g_globalState.windowHandle, 0, 0, PM_REMOVE))
		{
			switch (message.message)
			{
			case WM_QUIT: g_globalState.isRunning = false; break;
			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				uint32_t keyCode = message.wParam;
				bool isDown = !((message.lParam >> 31) & 0x1);

				switch (keyCode)
				{
				case 'W':
					g_globalState.wDown = isDown;
					break;
				case 'A':
					g_globalState.aDown = isDown;
					break;
				case 'S':
					g_globalState.sDown = isDown;
					break;
				case 'D':
					g_globalState.dDown = isDown;
					break;
				case 'Q':
					g_globalState.qDown = isDown;
					break;
				case 'E':
					g_globalState.eDown = isDown;
					break;
				}
			}
			default:
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
				break;
			}
			}
		}

		// All pixels are set to 0

		for (uint32_t y = 0; y < g_globalState.frameBufferHeight; ++y)
		{
			for (uint32_t x = 0; x < g_globalState.frameBufferWidth; ++x)
			{
				uint32_t pixelID = y * g_globalState.frameBufferWidth + x;

				unsigned __int8 alpha = 255;
				unsigned __int8 red = (unsigned __int8)0;
				unsigned __int8 green = (unsigned __int8)0;
				unsigned __int8 blue = (unsigned __int8)0;
				uint32_t pixelColor = (uint32_t)alpha << 24 | (uint32_t)red << 16 | (uint32_t)green << 8 | (uint32_t)blue;

				g_globalState.depthBuffer[pixelID] = FLT_MAX;
				g_globalState.frameBufferPixels[pixelID] = pixelColor;
			}
		}

		// Calculating camera position;
		M4 cameraTransform = identityM4();
		{
			Camera *p_camera = &g_globalState.camera;
			V3 frontMove = v3(0, 0, 1);
			V3 sideMove = v3(1, 0, 0);
			V3 verticalMove = v3(0, 1, 0);

			if (g_globalState.wDown)
			{
				p_camera->m_pos += frontMove * frameTime;
			}
			if (g_globalState.sDown)
			{
				p_camera->m_pos -= frontMove * frameTime;
			}
			if (g_globalState.aDown)
			{
				p_camera->m_pos -= sideMove * frameTime;
			}
			if (g_globalState.dDown)
			{
				p_camera->m_pos += sideMove * frameTime;
			}
			if (g_globalState.eDown)
			{
				p_camera->m_pos += verticalMove * frameTime; 
			}
			if (g_globalState.qDown)
			{
				p_camera->m_pos -= verticalMove * frameTime;
			}

			cameraTransform = translationMatrix(-p_camera->m_pos);
		}

		g_globalState.currentTime += frameTime;
		if (g_globalState.currentTime >= 2.0f * g_Pi)
			g_globalState.currentTime = 0.0f;

		// Creating a spinning cube		
		V3 cubeVertices[] =
		{
			v3(-0.5f, -0.5f, -0.5f),
			v3(-0.5f, 0.5f, -0.5f),
			v3(0.5f, 0.5f, -0.5f),
			v3(0.5f, -0.5f, -0.5f),


			v3(-0.5f, -0.5f, 0.5f),
			v3(-0.5f, 0.5f, 0.5f),
			v3(0.5f, 0.5f, 0.5f),
			v3(0.5f, -0.5f, 0.5f),
		};

		V3 cubeColors[] =
		{
			v3(1,0,0),
			v3(0,1,0),
			v3(0,0,1),
			v3(1,0,1),

			v3(1,1,0),
			v3(0,1,1),
			v3(1,0,1),
			v3(1,1,1),

		};

		uint32_t modelIndexes[] =
		{
			// Front
			0, 1, 2,
			2, 3, 0,
			// Back
			6, 5, 4,
			4, 7, 6,
			// Left
			4, 5, 1,
			1, 0, 4,
			//Right
			3, 2, 6,
			6, 7, 3,
			//Top
			1, 5, 6,
			6, 2, 1,
			//Bottom
			4, 0, 3,
			3, 7, 4,
		};

		M4 transform =  cameraTransform *
						translationMatrix(0, 0, 2) *
						rotationMatrix(g_globalState.currentTime, g_globalState.currentTime, g_globalState.currentTime) *
						scaleMatrix(1, 1, 1);

		for (uint32_t indexID = 0; indexID < std::size(modelIndexes); indexID += 3)
		{
			uint32_t index0 = modelIndexes[indexID + 0];
			uint32_t index1 = modelIndexes[indexID + 1];
			uint32_t index2 = modelIndexes[indexID + 2];

			drawTriangle(cubeVertices[index0], cubeVertices[index1], cubeVertices[index2],
				cubeColors[index0], cubeColors[index1], cubeColors[index2],
				transform);
		}

		RECT clientRect{};
		GetClientRect(g_globalState.windowHandle, &clientRect);
		uint32_t clientWidth = clientRect.right - clientRect.left;
		uint32_t clientHeight = clientRect.bottom - clientRect.top;

		BITMAPINFO bitMapInfo{};

		bitMapInfo.bmiHeader.biSize = sizeof(tagBITMAPINFOHEADER);
		bitMapInfo.bmiHeader.biWidth = g_globalState.frameBufferWidth;
		bitMapInfo.bmiHeader.biHeight = g_globalState.frameBufferHeight;
		bitMapInfo.bmiHeader.biPlanes = 1;
		bitMapInfo.bmiHeader.biBitCount = 32;
		bitMapInfo.bmiHeader.biCompression = BI_RGB;
		bitMapInfo.bmiHeader.biSizeImage = 0;

		Assert(StretchDIBits(
			g_globalState.deviceContext,
			0,
			0,
			clientWidth,
			clientHeight,
			0,
			0,
			g_globalState.frameBufferWidth,
			g_globalState.frameBufferHeight,
			g_globalState.frameBufferPixels.data(),
			&bitMapInfo,
			DIB_RGB_COLORS,
			SRCCOPY
		));

	}

	return 0;
}