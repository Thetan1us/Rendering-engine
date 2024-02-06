#include <vector>
#include <array>
#include <Windows.h>

#include "matrices.h"
#include "vectors.h"
#include "rendering-engine.h"

static GlobalState globalState;
static float Pi = 3.14159f;

V2 projectPoint(const V3 &m_pos)
{
	// NDC
	V2 result{};
	result = m_pos.m_xy / m_pos.m_z;
	// Pixels
	result = 0.5f * (result + v2(1.0f, 1.0f));
	result = result * v2(globalState.frameBufferWidth, globalState.frameBufferHeight);
	return result;
}

float vectorProduct(const V2 &a, const V2 &b)
{
	float result = a.m_x * b.m_y - a.m_y * b.m_x;
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
				unsigned int pixelID = y * globalState.frameBufferWidth + x;

				float t0 = -vectorLength1 / barycentricDiv;
				float t1 = -vectorLength2 / barycentricDiv;
				float t2 = -vectorLength0 / barycentricDiv;

				float depth = t0 * (1.0f / transformedPoint0.m_z) + t1 * (1.0f / transformedPoint1.m_z) + t2 * (1.0f / transformedPoint2.m_z);
				depth = 1.0f / depth;
				if (depth < globalState.depthBuffer[pixelID])
				{
					V3 finalColor = t0 * modelColor0 + t1 * modelColor1 + t2 * modelColor2;
					finalColor = finalColor * 255.0f;
					unsigned int finalColor32 = (unsigned int)0xFF << 24 | (unsigned int)finalColor.m_red << 16 | (unsigned int)finalColor.m_green << 8 | (unsigned int)finalColor.m_blue;

					globalState.frameBufferPixels[pixelID] = finalColor32;
					globalState.depthBuffer[pixelID] = depth;
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

		globalState.frameBufferWidth = 500;
		globalState.frameBufferHeight = 500;

		globalState.frameBufferPixels.resize(globalState.frameBufferHeight * globalState.frameBufferWidth);
		globalState.depthBuffer.resize(globalState.frameBufferHeight * globalState.frameBufferWidth);
	}

	LARGE_INTEGER startTime{};
	LARGE_INTEGER endTime{};
	Assert(QueryPerformanceCounter(&startTime));

	while (globalState.isRunning)
	{
		Assert(QueryPerformanceCounter(&endTime));
		float frameTime = float(endTime.QuadPart - startTime.QuadPart) / float(timerFrequency.QuadPart);
		startTime = endTime;

		bool wDown = false;
		bool aDown = false;
		bool sDown = false;
		bool dDown = false;
		bool qDown = false;
		bool eDown = false;
		MSG message{};
		while (PeekMessageA(&message, globalState.windowHandle, 0, 0, PM_REMOVE))
		{
			switch (message.message)
			{
			case WM_QUIT: globalState.isRunning = false; break;
			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				unsigned int keyCode = message.wParam;
				bool isDown = !(message.lParam >> 31);

				switch (keyCode)
				{
				case 'W':
					wDown = isDown;
					break;
				case 'A':
					aDown = isDown;
					break;
				case 'S':
					sDown = isDown;
					break;
				case 'D':
					dDown = isDown;
					break;
				case 'Q':
					qDown = isDown;
					break;
				case 'E':
					eDown = isDown;
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

		for (unsigned int y = 0; y < globalState.frameBufferHeight; ++y)
		{
			for (unsigned int x = 0; x < globalState.frameBufferWidth; ++x)
			{
				unsigned int pixelID = y * globalState.frameBufferWidth + x;

				unsigned __int8 alpha = 255;
				unsigned __int8 red = (unsigned __int8)0;
				unsigned __int8 green = (unsigned __int8)0;
				unsigned __int8 blue = (unsigned __int8)0;
				unsigned int pixelColor = (unsigned int)alpha << 24 | (unsigned int)red << 16 | (unsigned int)green << 8 | (unsigned int)blue;

				globalState.depthBuffer[pixelID] = FLT_MAX;
				globalState.frameBufferPixels[pixelID] = pixelColor;
			}
		}

		// Calculating camera position;
		/*M4 cameraTransform = identityM4();
		{
			Camera *p_camera = &globalState.camera;
			V3 frontMove = v3(0, 0, 1);
			V3 sideMove = v3(1, 0, 0);
			V3 verticalMove = v3(0, 1, 0);

			if (wDown)
			{
				p_camera->m_pos += (frontMove * frameTime);
			}
			if (sDown)
			{
				p_camera->m_pos -= (frontMove * frameTime);
			}
			if (aDown)
			{
				p_camera->m_pos += (sideMove * frameTime);
			}
			if (dDown)
			{
				p_camera->m_pos -= (sideMove * frameTime);
			}
			if (eDown)
			{
				p_camera->m_pos += (verticalMove * frameTime);
			}
			if (qDown)
			{
				p_camera->m_pos -= verticalMove * frameTime;
			}

			cameraTransform = translationMatrix(-p_camera->m_pos);
		}*/

		globalState.currentTime += frameTime;
		if (globalState.currentTime >= 2.0f * Pi)
			globalState.currentTime = 0.0f;

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

		unsigned int modelIndexes[] =
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

		M4 transform = translationMatrix(0, 0, 2) *
			rotationMatrix(globalState.currentTime, globalState.currentTime, globalState.currentTime) *
			scaleMatrix(1, 1, 1);

		for (unsigned int indexID = 0; indexID < std::size(modelIndexes); indexID += 3)
		{
			unsigned int index0 = modelIndexes[indexID + 0];
			unsigned int index1 = modelIndexes[indexID + 1];
			unsigned int index2 = modelIndexes[indexID + 2];

			drawTriangle(cubeVertices[index0], cubeVertices[index1], cubeVertices[index2],
				cubeColors[index0], cubeColors[index1], cubeColors[index2],
				transform);
		}

		RECT clientRect{};
		GetClientRect(globalState.windowHandle, &clientRect);
		unsigned int clientWidth = clientRect.right - clientRect.left;
		unsigned int clientHeight = clientRect.bottom - clientRect.top;

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