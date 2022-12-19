#include "TriangleRenderer.h"
#include "d3dx12.h"
#include <DirectXMath.h>
#include <stdexcept>

#include "SharedStruct.h"
using namespace DirectX;

TriangleRenderer::TriangleRenderer(Core* core)
{
	m_core = core;
}

bool TriangleRenderer::Init()
{
	auto device = m_core->GetDevice();
	Vertex vertices[3] = {};
	vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].Position = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	auto vertexStride = sizeof(Vertex);
	m_vertexBuffer = new VertexBuffer(device, vertexSize, vertexStride, vertices);
	if (!m_vertexBuffer->IsValid())
	{
		throw std::runtime_error("d");
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

	m_rootSignature = new RootSignature(device);
	if (!m_rootSignature->IsValid())
	{
		throw std::runtime_error("d");
	}
	m_pipelineState = new PipelineState();
	m_pipelineState->SetInputLayout(Vertex::InputLayout);
	m_pipelineState->SetRootSignature(m_rootSignature->Get());
	m_pipelineState->SetVS(L"../x64/Debug/SimpleVS2.cso");
	m_pipelineState->SetPS(L"../x64/Debug/SimplePS2.cso");
	m_pipelineState->Create(device);
	if (!m_pipelineState->IsValid())
	{
		throw std::runtime_error("d");
	}

	printf("�V�[���̏������ɐ���\n");
	return true;
}

void TriangleRenderer::Update(UniTransform tran)
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

void TriangleRenderer::Draw()
{
	auto currentIndex = m_core->m_frameIndex; // ���݂̃t���[���ԍ����擾����
	auto commandList = m_core->GetCommandList(); // �R�}���h���X�g
	auto vbView = m_vertexBuffer->View(); // ���_�o�b�t�@�r���[

	commandList->SetGraphicsRootSignature(m_rootSignature->Get()); // ���[�g�V�O�l�`�����Z�b�g
	commandList->SetPipelineState(m_pipelineState->Get()); // �p�C�v���C���X�e�[�g���Z�b�g
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer[currentIndex]->GetAddress()); // �萔�o�b�t�@���Z�b�g

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // �O�p�`��`�悷��ݒ�ɂ���
	commandList->IASetVertexBuffers(0, 1, &vbView); // ���_�o�b�t�@���X���b�g0�Ԃ��g����1�����ݒ肷��

	commandList->DrawInstanced(3, 1, 0, 0); // 3�̒��_��`�悷��
}
