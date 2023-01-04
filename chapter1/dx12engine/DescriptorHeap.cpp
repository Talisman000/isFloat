#include "DescriptorHeap.h"
#include "Texture2D.h"

const UINT HANDLE_MAX = 512;

DescriptorHeap::DescriptorHeap(Core* core)
{
	m_core = core;

	m_pHandles.clear();
	m_pHandles.reserve(HANDLE_MAX);

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NodeMask = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = HANDLE_MAX;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	auto device = m_core->GetDevice();

	// ディスクリプタヒープを生成
	auto hr = device->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf()));

	if (FAILED(hr))
	{
		m_IsValid = false;
		return;
	}

	m_IncrementSize = device->GetDescriptorHandleIncrementSize(desc.Type); // ディスクリプタヒープ1個のメモリサイズを返す
	m_IsValid = true;
}

ComPtr<ID3D12DescriptorHeap> DescriptorHeap::GetHeap()
{
	return m_pHeap;
}

std::shared_ptr<DescriptorHandle> DescriptorHeap::Register(Texture2D* texture)
{
	const auto count = m_pHandles.size();
	if (HANDLE_MAX <= count)
	{
		return nullptr;
	}

	auto pHandle = std::make_shared<DescriptorHandle>();

	auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart(); // ディスクリプタヒープの最初のアドレス
	handleCPU.ptr += m_IncrementSize * count; // 最初のアドレスからcount番目が今回追加されたリソースのハンドル

	auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); // ディスクリプタヒープの最初のアドレス
	handleGPU.ptr += m_IncrementSize * count; // 最初のアドレスからcount番目が今回追加されたリソースのハンドル

	pHandle->HandleCPU = handleCPU;
	pHandle->HandleGPU = handleGPU;

	auto device = m_core->GetDevice();
	auto resource = texture->Resource();
	auto desc = texture->ViewDesc();
	device->CreateShaderResourceView(resource.Get(), &desc, pHandle->HandleCPU); // シェーダーリソースビュー作成

	m_pHandles.push_back(pHandle);
	return pHandle; // ハンドルを返す
}
