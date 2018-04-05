// include the basic windows header files and the Direct3D header files
#include "stdafx.h"
#include <windows.h>
#include <d3d11.h>
#include <d3d9types.h>
#include "DebugFrameProvider.h"
#include "WebcamFrameProvider.h"
#include "DebugEffect.h"
#include "MediaFoundation.h"
#include <mfapi.h>
#include <mfidl.h>

#include "Direct3D.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(NULL,
		L"WindowClass",
		L"Direct3D Composition",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	auto devices = MediaFoundation::GetVideoDevices();
	for (auto it = devices.begin(); it != devices.end(); ++it) {
		OutputDebugString(L"Neil --");
		OutputDebugString(it->pFriendlyName);
	}

	Direct3D::GetInstance().Initialize(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT);
	Direct3D::GetInstance().Start();

	//WebcamFrameProvider *webcam = new WebcamFrameProvider();
	WebcamFrameProvider *webcam = new WebcamFrameProvider();
	DebugEffect *debugEffect = new DebugEffect();
	debugEffect->AddInputFrameProvider(webcam);

	DebugFrameProvider *debugFp = new DebugFrameProvider(300, 300);

	debugEffect->Initialize();
	//Direct3D::GetInstance().AddFrameProvider((FrameProvider<BGRAFrame> *)webcam, 0, 0);
	Direct3D::GetInstance().AddFrameProvider((FrameProvider<BGRAFrame> *)webcam, 0, 0);
	Direct3D::GetInstance().AddFrameProvider((FrameProvider<BGRAFrame> *)debugFp, 500, 20);

	MSG msg;

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
	}

	// clean up DirectX and COM
	Direct3D::GetInstance().Cleanup();

	return 0;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}