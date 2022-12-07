#include "ModelApp.h"
#include "streamreader.h";

void ModelApp::Prepare()
{
	PrepareShader();
	m_srvDescriptorBase = FrameBufferCount;
	CreateRootSignature();
	m_pipelineOpaque = CreateOpaquePSO();
	m_pipelineAlpha = CreateAlphaPSO();
	CreateCBV();
	CreateSampler();
}

void ModelApp::Cleanup()
{
	D3D12AppBase::Cleanup();
}

void ModelApp::MakeCommand(ComPtr<ID3D12GraphicsCommandList>& command)
{
	D3D12AppBase::MakeCommand(command);
}

void ModelApp::WaitGPU()
{
}

ComPtr<ID3D12Resource1> ModelApp::CreateBuffer(UINT bufferSize, const void* initialData)
{
}

void ModelApp::PrepareDescriptorHeapForModelApp(UINT materialCount)
{
}

void ModelApp::MakeModelGeometry(const Microsoft::glTF::Document& doc,
                                 std::shared_ptr<Microsoft::glTF::GLTFResourceReader> reader)
{
}

ComPtr<ID3D12PipelineState> ModelApp::CreateOpaquePSO()
{
}

ComPtr<ID3D12PipelineState> ModelApp::CreateAlphaPSO()
{
}

HRESULT ModelApp::PrepareShader()
{
	// シェーダーをコンパイル.
	HRESULT hr;
	ComPtr<ID3DBlob> errBlob;
	hr = CompileShaderFromFile(L"shaderVS.hlsl", L"vs_6_0", m_vs, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}
	hr = CompileShaderFromFile(L"shaderOpaquePS.hlsl", L"ps_6_0", m_psOpaque, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}
	hr = CompileShaderFromFile(L"shaderAlphaPS.hlsl", L"ps_6_0", m_psAlpha, errBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA((const char*)errBlob->GetBufferPointer());
	}
	return hr;
}

HRESULT ModelApp::CreateRootSignature()
{
	HRESULT hr;
	ComPtr<ID3DBlob> errBlob;
	CD3DX12_DESCRIPTOR_RANGE cbv, srv, sampler;
	cbv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0 レジスタ
	srv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
	sampler.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0); // s0 レジスタ

	CD3DX12_ROOT_PARAMETER rootParams[3];
	rootParams[0].InitAsDescriptorTable(1, &cbv, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParams[1].InitAsDescriptorTable(1, &srv, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParams[2].InitAsDescriptorTable(1, &sampler, D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャの構築
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.Init(
		_countof(rootParams), rootParams, //pParameters
		0, nullptr, //pStaticSamplers
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
	// RootSignature の生成
	hr = m_device->CreateRootSignature(
		0,
		signature->GetBufferPointer(), signature->GetBufferSize(),
		IID_PPV_ARGS(&m_rootSignature)
	);
	if (FAILED(hr))
	{
		throw std::runtime_error("CrateRootSignature Failed");
	}
	return hr;
}

HRESULT ModelApp::CreateCBV()
{
	m_constantBuffers.resize(FrameBufferCount);
	m_cbViews.resize(FrameBufferCount);
	for (UINT i = 0; i < FrameBufferCount; ++i)
	{
		// 定数バッファは255バイトのアラインメントが必要
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
	return S_OK;
}

HRESULT ModelApp::CreateSampler()
{
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

	auto descriptorSampler = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_heapSampler->GetCPUDescriptorHandleForHeapStart(),
		SamplerDescriptorBase,
		m_samplerDescriptorSize
	);

	m_device->CreateSampler(&samplerDesc, descriptorSampler);
	m_sampler = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		m_heapSrvCbv->GetGPUDescriptorHandleForHeapStart(),
		SamplerDescriptorBase,
		m_samplerDescriptorSize
	);
}
