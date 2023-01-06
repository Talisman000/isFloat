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
			printf("頂点バッファの生成に失敗\n");
			return false;
		}

		m_vertexBuffers.emplace_back(vertexBuffer);


		// インデックスバッファの生成
		auto size = sizeof(uint32_t) * std::size(mesh.Indices);
		auto indexBuffer = std::make_shared<IndexBuffer>(device, size, mesh.Indices.size(), mesh.Indices.data());
		if (!indexBuffer->IsValid())
		{
			printf("インデックスバッファの生成に失敗\n");
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


	const auto eyePos = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f); // 視点の位置
	const auto targetPos = XMVectorZero(); // 視点を向ける座標
	const auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
	constexpr auto fov = XMConvertToRadians(37.5); // 視野角
	const auto aspect = static_cast<float>(m_core->width) / static_cast<float>(m_core->height); // アスペクト比

	for (size_t i = 0; i < m_core->FrameBufferCount; i++)
	{
		m_constantBuffer[i] = std::make_shared<ConstantBuffer>(device, sizeof(ShaderProperty));
		if (!m_constantBuffer[i]->IsValid())
		{
			throw std::runtime_error("d");
		}

		// 変換行列の登録
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

	printf("シーンの初期化に成功\n");
	return true;
}

void MeshRenderer::Update(const Transform tran) const
{
	const auto currentIndex = m_core->m_frameIndex; // 現在のフレーム番号を取得
	const auto currentTransform = m_constantBuffer[currentIndex]->GetPtr<ShaderProperty>(); // 現在のフレーム番号に対応する定数バッファを取得
	//currentTransform->World = DirectX::XMMatrixRotationY(tran.Position.y); // Y軸で回転させる
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
	const auto currentIndex = m_core->m_frameIndex; // 現在のフレーム番号を取得する
	const auto commandList = m_core->GetCommandList(); // コマンドリスト
	commandList->SetGraphicsRootSignature(m_rootSignature->Get().Get());
	commandList->SetPipelineState(m_pipelineState->Get());
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer[currentIndex]->GetAddress());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const auto vbView = m_vertexBuffers[n]->View();
	const auto ibView = m_indexBuffers[n]->View(); // インデックスバッファビュー

	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView); // インデックスバッファをセットする

	auto pDescriptorHeap = m_descriptorHeap->GetHeap();
	commandList->SetDescriptorHeaps(1, pDescriptorHeap.GetAddressOf()); // 使用するディスクリプタヒープをセット
	commandList->SetGraphicsRootDescriptorTable(1, m_materialHandles[n]->HandleGPU); // そのメッシュに対応するディスクリプタテーブルをセット

	commandList->DrawIndexedInstanced(m_indexBuffers[n]->IndexCount(), 1, 0, 0, 0); // 6個のインデックスで描画する（三角形の時と関数名が違うので注意）
}
