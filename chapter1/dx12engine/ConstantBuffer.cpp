#include "ConstantBuffer.h"
#include <stdexcept>

ConstantBuffer::ConstantBuffer(const ComPtr<ID3D12Device>& device, size_t size)
{
	// 定数バッファは255の倍数に切り上げが必要
	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	UINT64 sizeAligned = (size + (align - 1)) & ~(align - 1); // alignに切り上げる.

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeAligned); // リソースの設定

	// リソースを生成
	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pBuffer.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		throw std::runtime_error("Create constant buffer resource failed");
	}

	hr = m_pBuffer->Map(0, nullptr, &m_pMappedPtr);
	if (FAILED(hr))
	{
		throw std::runtime_error("Map constant buffer failed");
	}

	m_Desc = {};
	m_Desc.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_Desc.SizeInBytes = UINT(sizeAligned);

	m_IsValid = true;
}

bool ConstantBuffer::IsValid() const
{
	return m_IsValid;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetAddress() const
{
	return m_Desc.BufferLocation;
}

D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBuffer::ViewDesc() const
{
	return m_Desc;
}

void* ConstantBuffer::GetPtr() const
{
	return m_pMappedPtr;
}
