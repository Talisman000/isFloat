#pragma once
#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>

#include "d3dx12.h"
#include "ComPtr.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
class Core
{
public:
	Core();
	virtual ~Core();

	void Initialize(HWND hwnd);
	void Terminate();

	//virtual void Render();

	virtual void Prepare()
	{
	}

	virtual void Cleanup();

	ID3D12Device* GetDevice();
	ID3D12GraphicsCommandList* GetCommandList();


	const UINT GpuWaitTimeout = (10 * 1000); // 10s
	const UINT FrameBufferCount = 2;
	int width = 0;
	int height = 0;
private:
	UINT m_dxgiFlags;
	ComPtr<ID3D12Device> m_device;
	ComPtr<IDXGIFactory3> m_factory;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<IDXGISwapChain4> m_swapchain;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;
	HANDLE m_fenceWaitEvent;
	std::vector<ComPtr<ID3D12Fence1>> m_frameFences;
	std::vector<UINT64> m_frameFenceValues;
	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;
	HRESULT EnableDebugLayer();
	HRESULT CreateDevice(); // デバイスを生成
	HRESULT CreateCommandQueue(); // コマンドキューを生成
	HRESULT CreateSwapChain(HWND hwnd); // スワップチェインを生成
	HRESULT CreateCommandList(); // コマンドリストとコマンドアロケーターを生成
	HRESULT CreateFence(); // フェンスを生成
	void CreateViewPort(); // ビューポートを生成
	void CreateScissorRect(); // シザー矩形を生成

private:
	std::vector<ComPtr<ID3D12Resource1>> m_renderTargets;
	UINT m_rtvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_heapRtv;
	UINT m_dsvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_heapDsv;
	ComPtr<ID3D12Resource1> m_depthBuffer;
private:
	void PrepareRenderTargetView();
	void CreateDepthBuffer();

public:
	UINT m_frameIndex;
	void BeginRender();
	void EndRender();
	void WaitPreviousFrame();
};

