#pragma once
#include "ComPtr.h"
#include <string>

#include "Core.h"
#include "d3dx12.h"

class DescriptorHeap;
class DescriptorHandle;

class Texture2D
{
public:
	static Texture2D* Get(Core* core, std::string path); // string�Ŏ󂯎�����p�X����e�N�X�`����ǂݍ���
	static Texture2D* Get(Core* core, std::wstring path); // wstring�Ŏ󂯎�����p�X����e�N�X�`����ǂݍ���
	static Texture2D* GetWhite(Core* core); // ���̒P�F�e�N�X�`���𐶐�����
	bool IsValid(); // ����ɓǂݍ��܂�Ă��邩�ǂ�����Ԃ�

	ID3D12Resource* Resource(); // ���\�[�X��Ԃ�
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc(); // �V�F�[�_�[���\�[�X�r���[�̐ݒ��Ԃ�

private:
	Core* m_core;
	bool m_IsValid; // ����ɓǂݍ��܂�Ă��邩
	Texture2D(Core* core, std::string path);
	Texture2D(Core* core, std::wstring path);
	Texture2D(Core* core, ID3D12Resource* buffer);
	ComPtr<ID3D12Resource> m_pResource; // ���\�[�X
	bool Load(std::string& path);
	bool Load(std::wstring& path);

	static ID3D12Resource* GetDefaultResource(Core* core, size_t width, size_t height);

	Texture2D(const Texture2D&) = delete;
	void operator = (const Texture2D&) = delete;
};
