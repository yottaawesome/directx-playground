#include "header.h"
#include <stdexcept>
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);
			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void RegisterWindowClass()
{
	WNDCLASSEX WndClass;
	static bool created = false;
	if (!created)
	{
		WndClass.cbSize = sizeof(WNDCLASSEX);
		WndClass.lpszClassName = L"Win32FrameworkDefault";
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		WndClass.lpfnWndProc = WndProc;
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hInstance = GetModuleHandle(nullptr);
		WndClass.hIcon = nullptr;
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		WndClass.lpszMenuName = nullptr;
		WndClass.hIconSm = nullptr;

		if (!RegisterClassEx(&WndClass))
			throw std::runtime_error("Failed class registration");
	}
}

HWND CreateMainWindow()
{
	HWND hWnd = CreateWindowEx
	(
		0,
		L"Win32FrameworkDefault", // class name
		L"Test", // window title
		WS_OVERLAPPEDWINDOW, // window styles
		0, // initial horizontal x position
		0,  // initial horizontal y position
		CW_USEDEFAULT,  // window width
		CW_USEDEFAULT, // window height
		nullptr, // parent HWND
		nullptr, // HWND menu/child
		GetModuleHandle(nullptr),//hInstance, // instance of the module
		(LPVOID)nullptr // additional data
	);

	if (hWnd == nullptr)
		throw std::runtime_error("Failed window creation");

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}