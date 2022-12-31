#include "IndexBuffer.h"
#include <stdexcept>
#include "d3dx12.h"

IndexBuffer::IndexBuffer(ID3D12Device* device, size_t size, const uint32_t* pInitData)
{
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // ヒーププロパティ
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);	// リソースの設定

	// リソースを生成
	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		std::runtime_error("Create index buffer resource Failed");
		return;
	}

	// インデックスバッファビューの設定
	m_View = {};
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.Format = DXGI_FORMAT_R32_UINT;
	m_View.SizeInBytes = static_cast<UINT>(size);

	// マッピングする
	if (pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			std::runtime_error("Index buffer mapping Failed");
			return;
		}

		// インデックスデータをマッピング先に設定
		memcpy(ptr, pInitData, size);

		// マッピング解除
		m_pBuffer->Unmap(0, nullptr);
	}
	m_IsValid = true;
}

bool IndexBuffer::IsValid()
{
	return m_IsValid;
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::View() const
{
	return m_View;
}
