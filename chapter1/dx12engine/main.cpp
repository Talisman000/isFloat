#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
//#include "D3D12AppBase.h"
//#include <stdexcept>
#include <atlstr.h>
//#include "CubeApp.h"
#include <iostream>
#include <random>

#include "AssimpLoader.h"
#include "Core.h"
#include "GameObject.h"
#include "MeshRenderer.h"
//#include "SampleTriangle.h"
#include "Time.h"
#include "CpComponents.h"
#include "KeyInput.h"
#include "Texture2D.h"
#include "Texture2DCache.h"
#include "XMFLOATHelper.h"
//#include "TriangleApp.h"

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
		auto hwnd = CreateWindow(wc.lpszClassName, L"Not_Gravity",
			dwStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left, rect.bottom - rect.top,
			nullptr,
			nullptr,
			hInstance,
			&core
		);
		core.Initialize(hwnd);

		auto world = GameObject::Create();

		std::vector<std::shared_ptr<GameObject>> backSquares;
		for (int i = 0; i < 30; i++)
		{
			//core.DebugSummary();
			auto sq = GameObject::Create();
			auto me = sq->AddComponent<CpMeshRenderer>();
			auto trimesh = SquareMesh({ 1,1,1,1 });
			me->SetMesh(&core, trimesh);
			sq->SetParent(world);

			auto rb = sq->AddComponent<CpRigidBody>();
			rb->isGravity = false;
			rb->velocity = {
				static_cast<float>(rand(mt) * -8) - 5,
				0,
				0
			};
			sq->transform.Position = {
				static_cast<float>(rand(mt) * 16) - 8,
				static_cast<float>(rand(mt) * 4),
				-2
			};
			auto s = 0.1f;
			sq->transform.Scale = {
				s,
				s * 0.1f,
				s,
			};
			backSquares.emplace_back(sq);
		}


		auto groundMesh = CubeMesh({ 0.2f,0.2f,0.2f,1.0f }, L"Assets/ground.tga");
		auto groundObj = GameObject::Create();
		auto cpMesh = groundObj->AddComponent<CpMeshRenderer>(-999);
		groundObj->SetParent(world);
		cpMesh->SetMesh(&core, groundMesh);
		groundObj->transform.Position = { 0,-2,0 };
		groundObj->transform.Scale = { 10,1,3 };

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&core));
		ShowWindow(hwnd, nCmdShow);

		MSG msg{};
		Time::Init();
		KeyInput::AddListen(VK_SPACE);
		while (msg.message != WM_QUIT)
		{
			Time::SetCurrent();
			float delta = Time::DeltaTime();
			KeyInput::Update();
			if (KeyInput::OnKeyDown(VK_SPACE))
			{
				OutputDebugString(L"space down");
			}
			if (KeyInput::OnKeyPress(VK_SPACE))
			{
				OutputDebugString(L"space press");
			}
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			core.BeginRender();


			for (auto& sq : backSquares)
			{
				if (sq->transform.Position.x < -8)
				{
					sq->transform.Position.x = 8;
				}
				sq->Update();
				sq->Draw();
			}


			groundObj->Update();
			groundObj->Draw();


			core.EndRender();
		}
		core.Cleanup();
		//CloseHandle();
		backSquares.clear();
		Texture2DCache::Clear();
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
