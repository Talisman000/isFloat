#pragma once
#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>

#include "d3dx12.h"
#include "ComPtr.h"
#include "RootSignature.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


// Coreクラス
// 各描画オブジェクトで共有できるものを管理
class Core
{
public:
	Core();
	virtual ~Core();

	void DebugSummary();

	void Initialize(HWND hwnd);
	void Cleanup();
	void Terminate();

	ComPtr<ID3D12Device> GetDevice();
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	std::shared_ptr<RootSignature> GetRootSignature(int n);


	const UINT GpuWaitTimeout = (10 * 1000); // 10s
	const UINT FrameBufferCount = 2;
	int width = 0;
	int height = 0;
private: // 描画に使用するDirectX12のオブジェクト
	// なるべく生ポインタを使わずスマートポインタComPtrを使用する

	UINT m_dxgiFlags;
	// デバイス
	ComPtr<ID3D12Device> m_device;

	// ファクトリー
	// スワップチェインやデバイスを作成するために使用
	ComPtr<IDXGIFactory3> m_factory;

	// スワップチェーン
	// フレームバッファをスワップするために必要
	ComPtr<IDXGISwapChain4> m_swapchain;

	// 描画コマンド関連

	// コマンドキュー
	// コマンドリストの待ち行列
	// コマンドリストに描画コマンドを貯める > コマンドキューにコマンドリストを入れる
	// > キューに溜まったリストを順次実行
	// の流れ
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	// コマンドリスト
	// 実際に描画コマンドを格納するとこ
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	// コマンドアロケータは
	// コマンドリストに積み込むためのバッファを確保するために必要
	// バッファごとに用意
	std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;

	// フェンス
	HANDLE m_fenceWaitEvent;
	// フェンス
	// バッファごとに作成
	std::vector<ComPtr<ID3D12Fence1>> m_frameFences;
	// フェンスの値
	// バッファごとに作成
	std::vector<UINT64> m_frameFenceValues;

	// ビューポート
	CD3DX12_VIEWPORT m_viewport;
	// シザー矩形
	CD3DX12_RECT m_scissorRect;

	std::vector<std::shared_ptr<RootSignature>> m_rootSignatures;

private: // 初期化関数
	HRESULT EnableDebugLayer();
	HRESULT CreateDevice(); // デバイスを生成
	HRESULT CreateCommandQueue(); // コマンドキューを生成
	HRESULT CreateSwapChain(HWND hwnd); // スワップチェインを生成
	HRESULT CreateCommandList(); // コマンドリストとコマンドアロケーターを生成
	HRESULT CreateFence(); // フェンスを生成
	void CreateViewPort(); // ビューポートを生成
	void CreateScissorRect(); // シザー矩形を生成
	void CreateRootSignatures();

private: // 描画に使用するオブジェクト
	// レンダーターゲット
	// レンダー（描画）するターゲット（そのまんま）
	// バッファの数だけ作成
	std::vector<ComPtr<ID3D12Resource1>> m_renderTargets;
	UINT m_rtvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_heapRtv;

	// デプスステンシル
	// 描画順とかを決めるのに必要
	UINT m_dsvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_heapDsv;
	ComPtr<ID3D12Resource1> m_depthBuffer;
private:
	void PrepareRenderTargetView();
	void CreateDepthBuffer();

public:
	UINT m_frameIndex;
	// 描画の流れ
	// BeginRender > 各描画オブジェクトの描画コマンド発行 > EndRender

	// 現在のフレームバッファの取得
	// 描画コマンド関連のオブジェクトのリセット
	// カラーバッファと深度ステンシルバッファの初期化
	void BeginRender();
	// ためた命令の実行
	// CPUとGPUの同期
	void EndRender();

	// CPUとGPUが同期するまで待つ
	void WaitPreviousFrame();
};

