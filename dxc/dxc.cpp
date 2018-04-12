#include <windows.h>
#include <d3d11.h>
#include <d3d9types.h>
#include <mfapi.h>
#include <mfidl.h>

#include "DebugFrameProvider.h"
#include "DebugEffect.h"
#include "Direct3D.h"
#include "game_capture_provider.h"
#include "WebcamFrameProvider.h"
#include "MediaFoundation.h"

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define DEFAULT_WIDTH  1280
#define DEFAULT_HEIGHT 720

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
    int nCmdShow) {
  HWND hwnd;
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

  hwnd = CreateWindowEx(NULL,
      L"WindowClass",
      L"Direct3D Composition",
      WS_OVERLAPPEDWINDOW,
      100,
      100,
      wr.right - wr.left,
      wr.bottom - wr.top,
      NULL,
      NULL,
      hInstance,
      NULL);

  ShowWindow(hwnd, nCmdShow);

  // enumerate cameras
  auto devices = MediaFoundation::GetVideoDevices();
  for (auto it = devices.begin(); it != devices.end(); ++it) {
    OutputDebugString(L"Neil --");
    OutputDebugString(it->pFriendlyName);
  }

  Direct3D::GetInstance().Initialize(hwnd, SCREEN_WIDTH, SCREEN_HEIGHT);
  Direct3D::GetInstance().Start();

  // WebcamFrameProvider *webcam = new WebcamFrameProvider();
  // DebugEffect* debugEffect = new DebugEffect();
  // debugEffect->AddInputFrameProvider(webcam);
  // debugEffect->Initialize();

  DebugFrameProvider* debug_fp = new DebugFrameProvider(300, 300);
  DebugFrameProvider* debug_fp_2 = new DebugFrameProvider(200, 100);
  GameCaptureProvider* gc_provider = new GameCaptureProvider(
      Direct3D::GetInstance().GetDevice(),
      Direct3D::GetInstance().GetDeviceContext(),
      1280, 720);

  // Direct3D::GetInstance().AddFrameProvider(webcam, 0, 0);
  Direct3D::GetInstance().AddFrameProvider(gc_provider, 0, 0);
  Direct3D::GetInstance().AddFrameProvider(debug_fp, 500, 20);
  Direct3D::GetInstance().AddFrameProvider(debug_fp_2, 100, 60);

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
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_DESTROY:
      {
        PostQuitMessage(0);
        return 0;
      } break;
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}
