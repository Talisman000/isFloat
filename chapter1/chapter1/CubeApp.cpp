#include "CubeApp.h"
#include <DirectXTex.h>
#include <stdexcept>

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
	rootParams[0].InitAsDescriptorTable(
		1,
		&cbv,
		D3D12_SHADER_VISIBILITY_VERTEX
	);
	rootParams[1].InitAsDescriptorTable(
		1,
		&srv,
		D3D12_SHADER_VISIBILITY_PIXEL
	);
	rootParams[2].InitAsDescriptorTable(
		1,
		&sampler,
		D3D12_SHADER_VISIBILITY_PIXEL
	);

	// construct root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.Init(
		_countof(rootParams),
		rootParams,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

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

	// create root signature
	hr = m_device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_rootSignature)
	);
	if (FAILED(hr))
	{
		throw std::runtime_error("CreateRootSignature Failed");
	}


	// create constant buffer
	m_constantBuffers.resize(FrameBufferCount);
	m_cbViews.resize(FrameBufferCount);
	for (UINT i = 0; i < FrameBufferCount; ++i)
	{
		UINT buffersize = sizeof(ShaderParameters) + 255 & ~255;
		m_constantBuffers[i] = CreateBuffer(buffersize, nullptr);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbDesc{};
		cbDesc.BufferLocation = m_constantBuffers[i]->GetGPUVirtualAddress();
		cbDesc.SizeInBytes = buffersize;
		CD3DX12_CPU_DESCRIPTOR_HANDLE handleCBV(
			m_heapSrvCbv->GetCPUDescriptorHandleForHeapStart(),
			ConstantBufferDescriptorBase + i,
			m_srvcbvDescriptorSize
		);
		m_device->CreateConstantBufferView(&cbDesc, handleCBV);
		m_cbViews[i] = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			m_heapSrvCbv->GetGPUDescriptorHandleForHeapStart(),
			ConstantBufferDescriptorBase + i,
			m_srvcbvDescriptorSize
		);
	}

	// create texture
	CreateTexture(L"texture.tga");

	D3D12_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D12_ENCODE_BASIC_FILTER(
		D3D12_FILTER_TYPE_LINEAR, // min
		D3D12_FILTER_TYPE_LINEAR, // mag
		D3D12_FILTER_TYPE_LINEAR, // mip
		D3D12_FILTER_REDUCTION_TYPE_STANDARD);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

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

HRESULT CubeApp::CreateTexture(const wchar_t* fileName)
{
	DirectX::ScratchImage image;
	DirectX::LoadFromTGAFile(fileName, nullptr, image);

	auto metadata = image.GetMetadata();
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	ComPtr<ID3D12Resource1> staging;
	ComPtr<ID3D12Resource> texture;

	// create texture;
	DirectX::CreateTexture(m_device.Get(), metadata, texture.ReleaseAndGetAddressOf());

	DirectX::PrepareUpload(
		m_device.Get(),
		image.GetImages(),
		image.GetImageCount(),
		metadata,
		subResources
	);

	texture.As(&m_texture);

	const auto totalBytes = GetRequiredIntermediateSize(m_texture.Get(), 0, subResources.size());
	const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(totalBytes);
	m_device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&staging)
	);

	ComPtr<ID3D12GraphicsCommandList> command;
	m_device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocators[m_frameIndex].Get(),
		nullptr, IID_PPV_ARGS(&command));
	ComPtr<ID3D12Fence1> fence;
	m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	UpdateSubresources(
		command.Get(),
		texture.Get(),
		staging.Get(),
		0,
		0,
		uint32_t(subResources.size()),
		subResources.data()
	);
	auto barrierTex = CD3DX12_RESOURCE_BARRIER::Transition(
		texture.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
	command->ResourceBarrier(1, &barrierTex);
	command->Close();

	// コマンドの実行
	ID3D12CommandList* cmds[] = { command.Get() };
	m_commandQueue->ExecuteCommandLists(1, cmds);

	// 完了したらシグナルを立てる.
	const UINT64 expected = 1;
	m_commandQueue->Signal(fence.Get(), expected);

	// テクスチャの処理が完了するまで待つ.
	while (expected != fence->GetCompletedValue())
	{
		Sleep(1);
	}

	//stbi_image_free(pImage);


	return S_OK;
}

void CubeApp::PrepareDescriptorHeapForCubeApp()
{
}
