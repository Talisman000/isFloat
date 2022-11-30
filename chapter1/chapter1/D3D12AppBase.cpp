#include "D3D12AppBase.h"
#include <exception>
#include <fstream>

#if _MSC_VER > 1922
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include <experimental/filesystem>


// DirectX Shader Compiler 用(第7章で説明)
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
D3D12AppBase::D3D12AppBase()
{
	m_renderTargets.resize(FrameBufferCount);
	m_frameFenceValues.resize(FrameBufferCount);
	m_frameIndex = 0;
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
#if 0 // GBV を有効化する場合
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
	}

	// Create Device
	hr = D3D12CreateDevice(useAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));

	D3D12_COMMAND_QUEUE_DESC queueDesc{
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		0,
		D3D12_COMMAND_QUEUE_FLAG_NONE,
		0
	};

	// Create CommandQueue
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
		hr = factory->CreateSwapChainForHwnd(
			m_commandQueue.Get(), hwnd,
			&scDesc, nullptr, nullptr, &swapchain
		);
		if (FAILED(hr))
		{
			throw std::runtime_error("CreateSwapChainForHwnd Failed");
		}
		swapchain.As(&m_swapchain);
	}

	PrepareDescriptorHeaps();

	PrepareRenderTargetView();

	CreateDepthBuffer(width, height);

	CreateCommandAllocators();

	CreateFrameFences();


	hr = m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocators[0].Get(),
		nullptr,
		IID_PPV_ARGS(&m_commandList)
	);

	m_commandList->Close();

	m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, float(width), float(height));
	m_scissorRect = CD3DX12_RECT(0, 0, LONG(width), LONG(height));

	Prepare();
}


void D3D12AppBase::Terminate()
{
}

void D3D12AppBase::Render()
{
	m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();

	m_commandAllocators[m_frameIndex]->Reset();
	m_commandList->Reset(
		m_commandAllocators[m_frameIndex].Get(),
		nullptr
	);

	// # render command start

	// barrier : swapchain -> rt
	auto barrierToRT = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrierToRT);


	// clear color buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
		m_heapRtv->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex, m_rtvDescriptorSize);
	const float clearColor[] = {0.1f, 0.25f, 0.5f, 0.0f}; // クリア色
	m_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

	// clear depth buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(
		m_heapDsv->GetCPUDescriptorHandleForHeapStart()
	);
	m_commandList->ClearDepthStencilView(
		dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// set render destination
	m_commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

	MakeCommand(m_commandList);

	// barrier rt -> swapchain
	auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
		m_renderTargets[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrierToPresent);

	// # render command end
	m_commandList->Close();

	ID3D12CommandList* lists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(1, lists);
	m_swapchain->Present(1, 0);
	WaitPreviousFrame();
}


void D3D12AppBase::PrepareDescriptorHeaps()
{
	// create rtv descriptor heap
	HRESULT hr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		FrameBufferCount,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0
	};
	hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_heapRtv));
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed CreateDescriptorHeap(RTV)");
	}
	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// create dsv descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		1,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		0
	};
	hr = m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_heapDsv));
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed CreateDescriptorHeap(DSV)");
	}
}

void D3D12AppBase::PrepareRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_heapRtv->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < FrameBufferCount; ++i)
	{
		m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		// change reference descriptor
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}
}

void D3D12AppBase::CreateDepthBuffer(int width, int height)
{
	HRESULT hr;
	// create depth buffer
	auto depthBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		width,
		height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.Format = depthBufferDesc.Format;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;

	// Depth BufferはCPUからのアクセスが不要で、GPUから頻繁にアクセスされるため、DEFAULTで確保する。
	const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	hr = m_device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&depthBufferDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&m_depthBuffer)
	);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed CreateCommittedResource(DepthBuffer)");
	}

	// create depth stencil view
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{
		DXGI_FORMAT_D32_FLOAT,
		D3D12_DSV_DIMENSION_TEXTURE2D,
		D3D12_DSV_FLAG_NONE,
		{
			0
		}
	};
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_heapDsv->GetCPUDescriptorHandleForHeapStart());
	m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, dsvHandle);
}

void D3D12AppBase::CreateCommandAllocators()
{
	HRESULT hr;
	m_commandAllocators.resize(FrameBufferCount);
	// バッファと同じ数だけCommandAllocatorを作る
	for (UINT i = 0; i < FrameBufferCount; ++i)
	{
		hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&m_commandAllocators[i]));
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed CreateCommandAllocator");
		}
	}
}

void D3D12AppBase::CreateFrameFences()
{
	HRESULT hr;
	m_frameFences.resize(FrameBufferCount);
	// バッファと同じ数だけFenceを作る
	// CPUとGPUの同期のために必要
	for (UINT i = 0; i < FrameBufferCount; ++i)
	{
		hr = m_device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_frameFences[i]));
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed CreateFence");
		}
	}
}

void D3D12AppBase::WaitPreviousFrame()
{
	auto& fence = m_frameFences[m_frameIndex];
	const auto currentValue = ++ m_frameFenceValues[m_frameIndex];
	m_commandQueue->Signal(fence.Get(), currentValue);

	auto nextIndex = (m_frameIndex + 1) % FrameBufferCount;
	const auto finishExpected = m_frameFenceValues[nextIndex];
	const auto nextFenceValue = m_frameFences[nextIndex]->GetCompletedValue();
	// GPUがまだ処理中の場合、CPU側は待機
	if (nextFenceValue < finishExpected)
	{
		m_frameFences[nextIndex]->SetEventOnCompletion(finishExpected, m_fenceWaitEvent);
		WaitForSingleObject(m_fenceWaitEvent, GpuWaitTimeout);
	}
}

HRESULT D3D12AppBase::CompileShaderFromFile(const std::wstring& fileName, const std::wstring& profile,
                                            ComPtr<ID3DBlob>& shaderBlob, ComPtr<ID3DBlob>& errorBlob)
{
	return S_OK;
}
