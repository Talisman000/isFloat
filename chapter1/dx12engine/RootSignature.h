#pragma once
#include "ComPtr.h"
#include "d3dx12.h"


class RootSignature
{
public:
	RootSignature(const ComPtr<ID3D12Device>& device); // �R���X�g���N�^�Ń��[�g�V�O�l�`���𐶐�
	bool IsValid(); // ���[�g�V�O�l�`���̐����ɐ����������ǂ�����Ԃ�
	ComPtr<ID3D12RootSignature> Get() const; // ���[�g�V�O�l�`����Ԃ�

private:
	bool m_IsValid = false; // ���[�g�V�O�l�`���̐����ɐ����������ǂ���
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr; // ���[�g�V�O�l�`��
};

