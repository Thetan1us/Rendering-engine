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

	WNDCLASSA windowClass{};
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = Win32WindowCallback;
	// Deleted cbClsExtra, cbWndExtra, hbrBackground, lpszMenuName, hIcon due to default 0 initialization
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

	while (globalState.isRunning)
	{
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
	}

		return 0;
}