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

	// �f�B�X�N���v�^�q�[�v�𐶐�
	auto hr = device->CreateDescriptorHeap(
		&desc,
		IID_PPV_ARGS(m_pHeap.ReleaseAndGetAddressOf()));

	if (FAILED(hr))
	{
		m_IsValid = false;
		return;
	}

	m_IncrementSize = device->GetDescriptorHandleIncrementSize(desc.Type); // �f�B�X�N���v�^�q�[�v1�̃������T�C�Y��Ԃ�
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

	auto handleCPU = m_pHeap->GetCPUDescriptorHandleForHeapStart(); // �f�B�X�N���v�^�q�[�v�̍ŏ��̃A�h���X
	handleCPU.ptr += m_IncrementSize * count; // �ŏ��̃A�h���X����count�Ԗڂ�����ǉ����ꂽ���\�[�X�̃n���h��

	auto handleGPU = m_pHeap->GetGPUDescriptorHandleForHeapStart(); // �f�B�X�N���v�^�q�[�v�̍ŏ��̃A�h���X
	handleGPU.ptr += m_IncrementSize * count; // �ŏ��̃A�h���X����count�Ԗڂ�����ǉ����ꂽ���\�[�X�̃n���h��

	pHandle->HandleCPU = handleCPU;
	pHandle->HandleGPU = handleGPU;

	auto device = m_core->GetDevice();
	auto resource = texture->Resource();
	auto desc = texture->ViewDesc();
	device->CreateShaderResourceView(resource.Get(), &desc, pHandle->HandleCPU); // �V�F�[�_�[���\�[�X�r���[�쐬

	m_pHandles.push_back(pHandle);
	return pHandle; // �n���h����Ԃ�
}
