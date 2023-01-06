#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <atlstr.h>
#include <random>

#include "Core.h"
#include "Time.h"
#include "GameScene.h"
#include "Texture2D.h"
#include "Texture2DCache.h"
#include "XMFLOATHelper.h"
//#include "dsound.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (msg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	try
	{
		//ModelApp theApp{};
		Core core{};
		//TriangleRenderer triangleRenderer(&core);
		//TriangleRenderer triangleRenderer2(&core);

		//Transform uniTransform;
		//Transform uniTransform2;

		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		std::random_device rnd;
		std::uniform_real_distribution<> rand(0, 1.0);
		std::mt19937 mt(rnd());

		// window class setting
		WNDCLASSEX wc{};
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName = L"HelloDirectX12";
		RegisterClassEx(&wc);

		DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX;
		RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		AdjustWindowRect(&rect, dwStyle, FALSE);
		auto hwnd = CreateWindow(wc.lpszClassName, L"!isFloat",
			dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top,
			nullptr,
			nullptr,
			hInstance,
			&core
		);
		core.Initialize(hwnd);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&core));
		ShowWindow(hwnd, nCmdShow);
		MSG msg{};
		{
			auto gameScene = GameScene(&core);
			gameScene.Start();
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				gameScene.Update();
				gameScene.Draw();
			}
			core.Cleanup();
			Texture2DCache::Clear();
		}
		core.Terminate();
		return static_cast<int>(msg.wParam);
	}
	catch (std::runtime_error e)
	{
		DebugBreak();
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
	}
	return 0;
}
