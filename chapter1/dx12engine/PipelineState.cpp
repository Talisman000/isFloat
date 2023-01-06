#include "PipelineState.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <stdexcept>

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState()
{
	// �p�C�v���C���X�e�[�g�̐ݒ�
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ���X�^���C�U�[�̓f�t�H���g
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O�͂Ȃ�
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // �u�����h�X�e�[�g���f�t�H���g
	auto& target = desc.BlendState.RenderTarget[0];
	target.BlendEnable = TRUE;
	target.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	target.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	target.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	target.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // �[�x�X�e���V���̓f�t�H���g���g��
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // �O�p�`��`��
	desc.NumRenderTargets = 1; // �`��Ώۂ�1
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1; // �T���v���[��1
	desc.SampleDesc.Quality = 0;
}

bool PipelineState::IsValid()
{
	return m_IsValid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ComPtr<ID3D12RootSignature> rootSignature)
{
	desc.pRootSignature = rootSignature.Get();
}

void PipelineState::SetVS(std::wstring filePath)
{
	// ���_�V�F�[�_�[�ǂݍ���
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pVsBlob.GetAddressOf());
	if (FAILED(hr))
	{
		throw std::runtime_error("Load vertex shader failed");
	}

	desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState::SetPS(std::wstring filePath)
{
	// �s�N�Z���V�F�[�_�[�ǂݍ���
	auto hr = D3DReadFileToBlob(filePath.c_str(), m_pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		throw std::runtime_error("Load pixel shader failed");
	}

	desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState::Create(ComPtr<ID3D12Device> device)
{
	// �p�C�v���C���X�e�[�g�𐶐�
	auto hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		throw std::runtime_error("Create pipeline state failed");
	}

	m_IsValid = true;
}

ID3D12PipelineState* PipelineState::Get()
{
	return m_pPipelineState.Get();
}
