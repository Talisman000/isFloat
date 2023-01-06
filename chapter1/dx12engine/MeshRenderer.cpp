#include "MeshRenderer.h"

#include "d3dx12.h"
#include <DirectXMath.h>
#include <stdexcept>

#include "SharedStruct.h"
#include <filesystem>

#include "DescriptorHeap.h"
#include "Texture2D.h"
#include "Texture2DCache.h"

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
		auto& diffuseMap = m_meshes[i].DiffuseMap;
		Texture2D* mainTex = nullptr;
		auto texPath = diffuseMap != L"" ? ReplaceExtension(diffuseMap, "tga") : L"";
		mainTex = Texture2DCache::Find(texPath);
		if (mainTex == nullptr)
		{
			mainTex = texPath != L"" ? Texture2D::Get(m_core, texPath):Texture2D::GetWhite(m_core);
			Texture2DCache::Register(texPath, mainTex);
		}
		auto handle = m_descriptorHeap->Register(mainTex);
		m_materialHandles.push_back(handle);
	}


	const auto eyePos = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f); // ���_�̈ʒu
	const auto targetPos = XMVectorZero(); // ���_����������W
	const auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // �������\���x�N�g��
	constexpr auto fov = XMConvertToRadians(37.5); // ����p
	const auto aspect = static_cast<float>(m_core->width) / static_cast<float>(m_core->height); // �A�X�y�N�g��

	for (size_t i = 0; i < m_core->FrameBufferCount; i++)
	{
		m_constantBuffer[i] = std::make_shared<ConstantBuffer>(device, sizeof(ShaderProperty));
		if (!m_constantBuffer[i]->IsValid())
		{
			throw std::runtime_error("d");
		}

		// �ϊ��s��̓o�^
		auto ptr = m_constantBuffer[i]->GetPtr<ShaderProperty>();
		ptr->World = XMMatrixIdentity();
		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	m_rootSignature = m_core->GetRootSignature(0);
	if (!m_rootSignature->IsValid())
	{
		throw std::runtime_error("d");
	}
	m_pipelineState = std::make_shared<PipelineState>();
	m_pipelineState->SetInputLayout(Vertex::InputLayout);
	m_pipelineState->SetRootSignature(m_rootSignature->Get());
	m_pipelineState->SetVS(L"./Assets/cso/VSBase.cso");
	m_pipelineState->SetPS(L"./Assets/cso/PSBase.cso");
	m_pipelineState->Create(device);
	if (!m_pipelineState->IsValid())
	{
		throw std::runtime_error("d");
	}

	printf("�V�[���̏������ɐ���\n");
	return true;
}

void MeshRenderer::Update(const Transform tran) const
{
	const auto currentIndex = m_core->m_frameIndex; // ���݂̃t���[���ԍ����擾
	const auto currentTransform = m_constantBuffer[currentIndex]->GetPtr<ShaderProperty>(); // ���݂̃t���[���ԍ��ɑΉ�����萔�o�b�t�@���擾
	//currentTransform->World = DirectX::XMMatrixRotationY(tran.Position.y); // Y���ŉ�]������
	currentTransform->World = XMMatrixScaling(tran.Scale.x, tran.Scale.y, tran.Scale.z) *
		XMMatrixRotationX(tran.Rotation.x) *
		XMMatrixRotationY(tran.Rotation.y) *
		XMMatrixRotationZ(tran.Rotation.z) *
		XMMatrixTranslation(tran.Position.x, tran.Position.y, tran.Position.z);
}


void MeshRenderer::Draw()
{
	for (int i = 0; i < m_meshes.size(); i++) {
		DrawIndexed(i);
	}

}

void MeshRenderer::DrawIndexed(const int n)
{
	if (m_meshes.size() <= n)
	{
		OutputDebugStringA("[Warning] DrawIndexed out of range\n");
	}
	const auto currentIndex = m_core->m_frameIndex; // ���݂̃t���[���ԍ����擾����
	const auto commandList = m_core->GetCommandList(); // �R�}���h���X�g
	commandList->SetGraphicsRootSignature(m_rootSignature->Get().Get());
	commandList->SetPipelineState(m_pipelineState->Get());
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer[currentIndex]->GetAddress());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const auto vbView = m_vertexBuffers[n]->View();
	const auto ibView = m_indexBuffers[n]->View(); // �C���f�b�N�X�o�b�t�@�r���[

	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView); // �C���f�b�N�X�o�b�t�@���Z�b�g����

	auto pDescriptorHeap = m_descriptorHeap->GetHeap();
	commandList->SetDescriptorHeaps(1, pDescriptorHeap.GetAddressOf()); // �g�p����f�B�X�N���v�^�q�[�v���Z�b�g
	commandList->SetGraphicsRootDescriptorTable(1, m_materialHandles[n]->HandleGPU); // ���̃��b�V���ɑΉ�����f�B�X�N���v�^�e�[�u�����Z�b�g

	commandList->DrawIndexedInstanced(m_indexBuffers[n]->IndexCount(), 1, 0, 0, 0); // 6�̃C���f�b�N�X�ŕ`�悷��i�O�p�`�̎��Ɗ֐������Ⴄ�̂Œ��Ӂj
}
