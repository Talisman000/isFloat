#include "MeshRenderer.h"

#include "d3dx12.h"
#include <DirectXMath.h>
#include <stdexcept>

#include "SharedStruct.h"
#include <filesystem>

#include "DescriptorHeap.h"
#include "Texture2D.h"

using namespace DirectX;

namespace fs = std::filesystem;
std::wstring ReplaceExtension(const std::wstring& origin, const char* ext)
{
	fs::path p = origin.c_str();
	return p.replace_extension(ext).c_str();
}
MeshRenderer::MeshRenderer(Core* core, Mesh mesh)
{
	m_core = core;
	m_meshes.emplace_back(mesh);
	Init();
}

MeshRenderer::MeshRenderer(Core* core, std::vector<Mesh> meshes)
{
	m_core = core;
	m_meshes = meshes;
	Init();
}

bool MeshRenderer::Init()
{
	auto device = m_core->GetDevice();

	for (auto& mesh : m_meshes)
	{
		auto vertexSize = sizeof(Vertex) * std::size(mesh.Vertices);
		auto vertexStride = sizeof(Vertex);
		auto vertexBuffer = std::make_shared<VertexBuffer>(device, vertexSize, vertexStride, mesh.Vertices.data());
		if (!vertexBuffer->IsValid())
		{
			printf("���_�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}

		m_vertexBuffers.emplace_back(vertexBuffer);


		// �C���f�b�N�X�o�b�t�@�̐���
		auto size = sizeof(uint32_t) * std::size(mesh.Indices);
		auto indexBuffer = std::make_shared<IndexBuffer>(device, size, mesh.Indices.size(), mesh.Indices.data());
		if (!indexBuffer->IsValid())
		{
			printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
			return false;
		}

		m_indexCount = mesh.Indices.size();
		m_indexBuffers.emplace_back(indexBuffer);
	}
	m_descriptorHeap = std::make_shared<DescriptorHeap>(m_core);

	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		if (m_meshes[i].DiffuseMap != L"")
		{
			auto texPath = ReplaceExtension(m_meshes[i].DiffuseMap, "tga"); // ���Ƃ��Ƃ�psd�ɂȂ��Ă��Ă�����Ƃ߂�ǂ������̂ŁA��������Ă���tga��ǂݍ���
			auto mainTex = Texture2D::Get(m_core, texPath);
			auto handle = m_descriptorHeap->Register(mainTex);
			m_materialHandles.push_back(handle);
		}
		else
		{
			auto mainTex = Texture2D::GetWhite(m_core);
			auto handle = m_descriptorHeap->Register(mainTex);
			m_materialHandles.push_back(handle);
		}
	}


	auto eyePos = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f); // ���_�̈ʒu
	auto targetPos = XMVectorZero(); // ���_����������W
	auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // �������\���x�N�g��
	auto fov = XMConvertToRadians(37.5); // ����p
	auto aspect = static_cast<float>(m_core->width) / static_cast<float>(m_core->height); // �A�X�y�N�g��

	for (size_t i = 0; i < m_core->FrameBufferCount; i++)
	{
		m_constantBuffer[i] = new ConstantBuffer(device, sizeof(Transform));
		if (!m_constantBuffer[i]->IsValid())
		{
			throw std::runtime_error("d");
		}

		// �ϊ��s��̓o�^
		auto ptr = m_constantBuffer[i]->GetPtr<Transform>();
		ptr->World = XMMatrixIdentity();
		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	m_rootSignature = m_core->GetRootSignature(0).get();
	if (!m_rootSignature->IsValid())
	{
		throw std::runtime_error("d");
	}
	m_pipelineState = new PipelineState();
	m_pipelineState->SetInputLayout(Vertex::InputLayout);
	m_pipelineState->SetRootSignature(m_rootSignature->Get());
	m_pipelineState->SetVS(L"./SimpleVS2.cso");
	m_pipelineState->SetPS(L"./SimplePS2.cso");
	m_pipelineState->Create(device);
	if (!m_pipelineState->IsValid())
	{
		throw std::runtime_error("d");
	}

	printf("�V�[���̏������ɐ���\n");
	return true;
}

void MeshRenderer::Update(RenderProperty tran)
{
	auto currentIndex = m_core->m_frameIndex; // ���݂̃t���[���ԍ����擾
	auto currentTransform = m_constantBuffer[currentIndex]->GetPtr<Transform>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾
	//currentTransform->World = DirectX::XMMatrixRotationY(tran.Position.y); // Y���ŉ�]������
	currentTransform->World = XMMatrixScaling(tran.Scale.x, tran.Scale.y, tran.Scale.z) *
		XMMatrixRotationX(tran.Rotation.x) *
		XMMatrixRotationY(tran.Rotation.y) *
		XMMatrixRotationZ(tran.Rotation.z) *
		XMMatrixTranslation(tran.Position.x, tran.Position.y, tran.Position.z);
}

void MeshRenderer::Draw()
{
	auto currentIndex = m_core->m_frameIndex; // ���݂̃t���[���ԍ����擾����
	auto commandList = m_core->GetCommandList(); // �R�}���h���X�g


	for (int i = 0; i < m_meshes.size(); i++) {
		commandList->SetGraphicsRootSignature(m_rootSignature->Get());
		commandList->SetPipelineState(m_pipelineState->Get());
		commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer[currentIndex]->GetAddress());

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		auto vbView = m_vertexBuffers[i]->View();
		auto ibView = m_indexBuffers[i]->View(); // �C���f�b�N�X�o�b�t�@�r���[

		commandList->IASetVertexBuffers(0, 1, &vbView);
		commandList->IASetIndexBuffer(&ibView); // �C���f�b�N�X�o�b�t�@���Z�b�g����

		auto pDescriptorHeap = m_descriptorHeap->GetHeap();
		commandList->SetDescriptorHeaps(1, &pDescriptorHeap); // �g�p����f�B�X�N���v�^�q�[�v���Z�b�g
		commandList->SetGraphicsRootDescriptorTable(1, m_materialHandles[i]->HandleGPU); // ���̃��b�V���ɑΉ�����f�B�X�N���v�^�e�[�u�����Z�b�g

		commandList->DrawIndexedInstanced(m_indexBuffers[i]->IndexCount(), 1, 0, 0, 0); // 6�̃C���f�b�N�X�ŕ`�悷��i�O�p�`�̎��Ɗ֐������Ⴄ�̂Œ��Ӂj
	}

}
