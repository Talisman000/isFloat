#include "CubeApp.h"

void CubeApp::Prepare()
{
	const float k = 1.0f;
	constexpr DirectX::XMFLOAT4 red(1.0f, 0.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 green(0.0f, 1.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 blue(0.0f, 0.0f, 1.0f, 1.0);
	constexpr DirectX::XMFLOAT4 white(1.0f, 1.0f, 1.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 magenta(1.0f, 0.0f, 1.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 cyan(0.0f, 1.0f, 1.0f, 1.0f);
	Vertex cubeVertices[] = {
		// 正面
		{{-k, -k, -k}, red, {0.0f, 1.0f}},
		{{-k, k, -k}, yellow, {0.0f, 0.0f}},
		{{k, k, -k}, white, {1.0f, 0.0f}},
		{{k, -k, -k}, magenta, {1.0f, 1.0f}},
		// 右
		{{k, -k, -k}, magenta, {0.0f, 1.0f}},
		{{k, k, -k}, white, {0.0f, 0.0f}},
		{{k, k, k}, cyan, {1.0f, 0.0f}},
		{{k, -k, k}, blue, {1.0f, 1.0f}},
		// 左
		{{-k, -k, k}, black, {0.0f, 1.0f}},
		{{-k, k, k}, green, {0.0f, 0.0f}},
		{{-k, k, -k}, yellow, {1.0f, 0.0f}},
		{{-k, -k, -k}, red, {1.0f, 1.0f}},
		// 裏
		{{k, -k, k}, blue, {0.0f, 1.0f}},
		{{k, k, k}, cyan, {0.0f, 0.0f}},
		{{-k, k, k}, green, {1.0f, 0.0f}},
		{{-k, -k, k}, black, {1.0f, 1.0f}},
		// 上
		{{-k, k, -k}, yellow, {0.0f, 1.0f}},
		{{-k, k, k}, green, {0.0f, 0.0f}},
		{{k, k, k}, cyan, {1.0f, 0.0f}},
		{{k, k, -k}, white, {1.0f, 1.0f}},
		// 底
		{{-k, -k, k}, red, {0.0f, 1.0f}},
		{{-k, -k, -k}, red, {0.0f, 0.0f}},
		{{k, -k, -k}, magenta, {1.0f, 0.0f}},
		{{k, -k, k}, blue, {1.0f, 1.0f}},
	};
	uint32_t indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20,
	};

	// create vertex buffer and index buffer
	m_vertexBuffer = CreateBuffer(sizeof(cubeVertices), cubeVertices);
	m_indexBuffer = CreateBuffer(sizeof(indices), indices);
	m_indexCount = _countof(indices);

	// create buffer view
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(cubeVertices);
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = sizeof(indices);
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// compile shader
	HRESULT hr;
	ComPtr<ID3DBlob> errBlob;
	hr = CompileShaderFromFile(L"sampleTexVS.hlsl", L"vs_6_0", m_vs, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}
	hr = CompileShaderFromFile(L"sampleTexPS.hlsl", L"ps_6_0", m_ps, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}

	CD3DX12_DESCRIPTOR_RANGE cbv, srv, sampler;
	cbv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	srv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	sampler.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	CD3DX12_ROOT_PARAMETER rootParams[3];
}

void CubeApp::Cleanup()
{
	D3D12AppBase::Cleanup();
}

void CubeApp::MakeCommand(ComPtr<ID3D12GraphicsCommandList>& command)
{
	D3D12AppBase::MakeCommand(command);
}

ComPtr<ID3D12Resource1> CubeApp::CreateBuffer(UINT bufferSize, const void* initialData)
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

	// 初期データの指定があるときにはコピーする
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

ComPtr<ID3D12Resource1> CubeApp::CreateTexture(const std::string& fileName)
{
}

void CubeApp::PrepareDescriptorHeapForCubeApp()
{
}
