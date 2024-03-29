#pragma once
#include "ComPtr.h"
#include "d3dx12.h"
#include <vector>

#include "Core.h"

class ConstantBuffer;
class Texture2D;

class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;
};

class DescriptorHeap
{
public:
	DescriptorHeap(Core* core); // コンストラクタで生成する
	ComPtr<ID3D12DescriptorHeap> GetHeap(); // ディスクリプタヒープを返す
	std::shared_ptr<DescriptorHandle> Register(Texture2D* texture); // テクスチャーをディスクリプタヒープに登録し、ハンドルを返す

private:
	Core* m_core;
	bool m_IsValid = false; // 生成に成功したかどうか
	UINT m_IncrementSize = 0;
	ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr; // ディスクリプタヒープ本体
	std::vector<std::shared_ptr<DescriptorHandle>> m_pHandles = {}; // 登録されているハンドル
};
