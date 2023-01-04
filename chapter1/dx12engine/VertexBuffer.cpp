#include "VertexBuffer.h"
#include "d3dx12.h"
#include <stdexcept>

VertexBuffer::VertexBuffer(ComPtr<ID3D12Device> device, size_t size, size_t stride, const void* pInitData)
{

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 	// �q�[�v�v���p�e�B
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(size); 	// ���\�[�X�̐ݒ�

	// ���\�[�X�𐶐�
	auto hr = device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pBuffer.ReleaseAndGetAddressOf()));

	if (FAILED(hr))
	{
		throw std::runtime_error("Create index buffer resource failed");
	}

	// ���_�o�b�t�@�r���[�̐ݒ�
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.SizeInBytes = static_cast<UINT>(size);
	m_View.StrideInBytes = static_cast<UINT>(stride);

	// �}�b�s���O����
	if (pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			throw std::runtime_error("Mapping index buffer failed");
		}

		// ���_�f�[�^���}�b�s���O��ɐݒ�
		memcpy(ptr, pInitData, size);

		// �}�b�s���O����
		m_pBuffer->Unmap(0, nullptr);
	}

	m_IsValid = true;
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::View() const
{
	return m_View;
}

bool VertexBuffer::IsValid() const
{
	return m_IsValid;
}

