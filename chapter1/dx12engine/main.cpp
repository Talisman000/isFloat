#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
//#include "D3D12AppBase.h"
//#include <stdexcept>

//#include "CubeApp.h"
#include <random>

#include "AssimpLoader.h"
#include "Core.h"
#include "MeshRenderer.h"
#include "SampleTriangle.h"
//#include "TriangleApp.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

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
	//ModelApp theApp{};
	Core core{};
	//TriangleRenderer triangleRenderer(&core);
	//TriangleRenderer triangleRenderer2(&core);

	//RenderProperty uniTransform;
	//RenderProperty uniTransform2;

	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	std::random_device rnd;
	std::uniform_real_distribution<> rand(-3.0, 3.0);
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
	auto hwnd = CreateWindow(wc.lpszClassName, L"HelloDirectX12",
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		&core
	);
	try
	{
		core.Initialize(hwnd);
		std::vector<SampleTriangle> triangles;
		for (int i = 0; i < 10; i++)
		{
			SampleTriangle tri(&core);
			tri.transform.Position = {
				static_cast<float>(rand(mt)),
				static_cast<float>(rand(mt)),
				0
			};
			tri.transform.Rotation = {
				0,
				0,
				static_cast<float>(rand(mt))
			};
			auto s = std::abs(static_cast<float>(rand(mt))) * 0.5f;
			tri.transform.Scale = {
				s,
				s,
				s,
			};
			triangles.emplace_back(tri);
		}

		auto squareMesh = SquareMesh({1,1,1,1}, L"Assets/men.png");
		auto mesh = MeshRenderer(&core, squareMesh);
		//auto cubeMesh = CubeMesh({1,1,1,1}, L"Assets/test.png");
		//auto mesh = MeshRenderer(&core, cubeMesh);
		RenderProperty rp;
		//const wchar_t* modelFile = L"Assets/Alicia/FBX/Alicia_solid_Unity.FBX";

		//std::vector<Mesh> meshes;
		//ImportSettings importSetting = // これ自体は自作の読み込み設定構造体
		//{
		//	modelFile,
		//	meshes,
		//	false,
		//	true // アリシアのモデルは、テクスチャのUVのVだけ反転してるっぽい？ので読み込み時にUV座標を逆転させる
		//};

		//AssimpLoader loader;
		//if (!loader.Load(importSetting))
		//{
		//	throw std::runtime_error("model load failed");
		//}

		//auto modelRenderer = MeshRenderer(&core, meshes);
		//auto modelPos = RenderProperty();
		//modelPos.Position.z = -200;
		//modelPos.Position.y = -100;


		//theApp.Initialize(hwnd);
		//triangleRenderer.Init();
		//triangleRenderer2.Init();

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&core));
		ShowWindow(hwnd, nCmdShow);

		MSG msg{};
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			core.BeginRender();

			//uniTransform.Position.x += 0.01f;
			//uniTransform.Rotation.x += 0.01f;
			for (auto& tri : triangles)
			{
				tri.transform.Rotation.z += 0.01f;
				tri.transform.Position.x += -0.01f;
				if (tri.transform.Position.x < -5)
				{
					tri.transform.Position.x = 5;
				}
				tri.Draw();
			}

			rp.Rotation.x += 0.01f;
			rp.Rotation.y += 0.01f;
			rp.Position.x -= 0.01f;
			if (rp.Position.x < -5)
			{
				rp.Position.x = 5;
			}

			mesh.Update(rp);

			mesh.Draw();

			//modelRenderer.Update(modelPos);
			//modelRenderer.Draw();

			core.EndRender();
			//theApp.Render();
		}
		core.Cleanup();
		//CloseHandle();
		//theApp.Terminate();
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
