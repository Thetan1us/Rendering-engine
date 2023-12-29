
#include <Windows.h>

#include "rendering-engine.h"

static GlobalState globalState;

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
			1366,
			768,
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
		float frameTime = float(endTime.QuadPart - startTime.QuadPart) / float(timerFrequency.QuadPart);
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

		float speed = 200.0f;
		globalState.currentOffset += speed * frameTime;

		for (u32 y = 0; y < globalState.frameBufferHeight; ++y)
		{
			for (u32 x = 0; x < globalState.frameBufferWidth; ++x)
			{
				u32 pixelID = y * globalState.frameBufferWidth + x;

				u8 alpha = 255;
				u8 red = 0;
				u8 green = (u8)(y + globalState.currentOffset);
				u8 blue = (u8)(x - globalState.currentOffset);
				u32 pixelColor = (u32)alpha << 24 | (u32)red << 16 | (u32)green << 8 | (u32)blue;

				globalState.frameBufferPixels[pixelID] = pixelColor;
			}
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