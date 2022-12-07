#include "TriangleApp.h"
#include <stdexcept>

void TriangleApp::Prepare()
{
	Vertex triangleVertices[] = {
		{{0.0f, 0.25f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.25f, -0.25f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.25f, -0.25f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	};
	uint32_t indices[] = { 0, 1, 2 };

	// create vertex buffer & index buffer
	m_vertexBuffer = CreateBuffer(sizeof(triangleVertices), triangleVertices);
	m_indexBuffer = CreateBuffer(sizeof(indices), indices);
	m_indexCount = _countof(indices);


	// create buffer view
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(triangleVertices);
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = sizeof(indices);
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// compile shader
	HRESULT hr;
	ComPtr<ID3DBlob> errBlob;
	hr = CompileShaderFromFile(L"simpleVS.hlsl", L"vs_6_0", m_vs, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}
	hr = CompileShaderFromFile(L"simplePS.hlsl", L"ps_6_0", m_ps, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}


	// create root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> signature;

	hr = D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&signature,
		&errBlob
	);
	if (FAILED(hr))
	{
		throw std::runtime_error("D3D12SerializeRootSignature Failed");
	}

	hr = m_device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_rootSignature)
	);

	if (FAILED(hr))
	{
		throw std::runtime_error("Create Root Signature Failed");
	}


	// input layout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{
			"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,
			0, offsetof(Vertex,Pos),
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, offsetof(Vertex,Color),
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}

	};

	// pipeline state desc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	// �V�F�[�_�[�̃Z�b�g
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vs.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_ps.Get());
	// �u�����h�X�e�[�g�ݒ�
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// ���X�^���C�U�[�X�e�[�g
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	// �o�͐��1�^�[�Q�b�g
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �f�v�X�o�b�t�@�̃t�H�[�}�b�g��ݒ�
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	// ���[�g�V�O�l�`���̃Z�b�g
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �}���`�T���v���ݒ�
	psoDesc.SampleDesc = { 1,0 };
	psoDesc.SampleMask = UINT_MAX; // �����Y���ƊG���o�Ȃ����x�����o�Ȃ��̂Œ���.

	hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipeline));
	if (FAILED(hr))
	{
		throw std::runtime_error("CreateGraphicsPipelineState Failed");
	}
}

void TriangleApp::Cleanup()
{
}

void TriangleApp::MakeCommand(ComPtr<ID3D12GraphicsCommandList>& command)
{
	command->SetPipelineState(m_pipeline.Get());
	command->SetGraphicsRootSignature(m_rootSignature.Get());
	command->RSSetViewports(1, &m_viewport);
	command->RSSetScissorRects(1, &m_scissorRect);

	command->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	command->IASetIndexBuffer(&m_indexBufferView);

	command->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}

ComPtr<ID3D12Resource1> TriangleApp::CreateBuffer(UINT bufferSize, const void* initialData)
{
	  HRESULT hr;
  ComPtr<ID3D12Resource1> buffer;
  const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  const auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
  hr = m_device->CreateCommittedResource(
    &heapProps,
    D3D12_HEAP_FLAG_NONE,
    &resDesc,
    D3D12_RESOURCE_STATE_GENERIC_READ,
    nullptr,
    IID_PPV_ARGS(&buffer)
  );

  // �����f�[�^�̎w�肪����Ƃ��ɂ̓R�s�[����
  if (SUCCEEDED(hr) && initialData != nullptr)
  {
    void* mapped;
    CD3DX12_RANGE range(0, 0);
    hr = buffer->Map(0, &range, &mapped);
    if (SUCCEEDED(hr))
    {
      memcpy(mapped, initialData, bufferSize);
      buffer->Unmap(0, nullptr);
    }
  }

  return buffer;
}