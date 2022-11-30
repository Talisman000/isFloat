#include "D3D12AppBase.h"
#include "D3D12AppBase.h"
#include <exception>
#include <fstream>

#if _MSC_VER > 1922
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>


// DirectX Shader Compiler óp(ëÊ7èÕÇ≈ê‡ñæ)
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
D3D12AppBase::D3D12AppBase()
{
}

D3D12AppBase::~D3D12AppBase()
{
}

void D3D12AppBase::Initialize(HWND hwnd)
{
	HRESULT hr;
	UINT dxgiFlags = 0;
	// debug layer
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debug;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
	{
		debug->EnableDebugLayer();
		dxgiFlags |= DXGI_CREATE_FACTORY_DEBUG;
#if 0 // GBV ÇóLå¯âªÇ∑ÇÈèÍçá
		ComPtr<ID3D12Debug3> debug3;
		debug.As(&debug3);
		if (debug3)
		{
			debug3->SetEnableGPUBasedValidation(true);
		}
#endif
	}
#endif
	// Create DXGIFactory
	ComPtr<IDXGIFactory3> factory;
	hr = CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&factory));
	if (FAILED(hr))
	{
		throw std::runtime_error("CreateDXGIFactory2 Failed");
	}
	// Search Hardware Adapter
	ComPtr<IDXGIAdapter1> useAdapter;
	{
		UINT adapterIndex{};
		ComPtr<IDXGIAdapter1> adapter;
		while (DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter))
		{
			DXGI_ADAPTER_DESC1 desc{};
			adapter->GetDesc1(&desc);
			++adapterIndex;
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			// available d3d12?
			hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(hr))
				break;
		}
		adapter.As(&useAdapter);

		hr = D3D12CreateDevice(useAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));

		D3D12_COMMAND_QUEUE_DESC queueDesc{
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			0,
			D3D12_COMMAND_QUEUE_FLAG_NONE,
			0
		};
		hr = m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
		if (FAILED(hr))
		{
			throw std::runtime_error("CreateCommandQueue Failed");
		}


		RECT rect;
		GetClientRect(hwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		// Create SwapChain
		{
			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			scDesc.BufferCount = FrameBufferCount;
			scDesc.Width = width;
			scDesc.Height = height;
			scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scDesc.SampleDesc.Count = 1;

			ComPtr<IDXGISwapChain1> swapchain;
			factory->CreateSwapChainForHwnd(
				m_commandQueue.Get(), hwnd,
				&scDesc, nullptr, nullptr, &swapchain
			);
			swapchain.As(&m_swapchain);
		}
	}


}

void D3D12AppBase::Terminate()
{
}

void D3D12AppBase::Render()
{
}

void D3D12AppBase::PrepareDescriptorHeaps()
{

}
