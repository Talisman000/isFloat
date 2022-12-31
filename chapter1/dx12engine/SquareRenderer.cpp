#include "SquareRenderer.h"

#include "d3dx12.h"
#include <DirectXMath.h>
#include <stdexcept>

#include "SharedStruct.h"

using namespace DirectX;

SquareRenderer::SquareRenderer(Core* core)
{
	m_core = core;
}

bool SquareRenderer::Init()
{
	auto device = m_core->GetDevice();
		// 頂点を4つにして四角形を定義する
	Vertex vertices[4] = {};
	vertices[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[1].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	vertices[3].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[3].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	auto vertexSize = sizeof(Vertex) * std::size(vertices);
	auto vertexStride = sizeof(Vertex);
	m_vertexBuffer = new VertexBuffer(device, vertexSize, vertexStride, vertices);
	if (!m_vertexBuffer->IsValid())
	{
		printf("頂点バッファの生成に失敗\n");
		return false;
	}

	uint32_t indices[] = { 0, 1, 2, 0, 2, 3 }; // これに書かれている順序で描画する

	// インデックスバッファの生成
	auto size = sizeof(uint32_t) * std::size(indices);
	m_indexBuffer = new IndexBuffer(device, size, indices);
	if (!m_indexBuffer->IsValid())
	{
		printf("インデックスバッファの生成に失敗\n");
		return false;
	}

	auto eyePos = XMVectorSet(0.0f, 0.0f, 10.0f, 0.0f); // 視点の位置
	auto targetPos = XMVectorZero(); // 視点を向ける座標
	auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
	auto fov = XMConvertToRadians(37.5); // 視野角
	auto aspect = static_cast<float>(m_core->width) / static_cast<float>(m_core->height); // アスペクト比

	for (size_t i = 0; i < m_core->FrameBufferCount; i++)
	{
		m_constantBuffer[i] = new ConstantBuffer(device, sizeof(Transform));
		if (!m_constantBuffer[i]->IsValid())
		{
			throw std::runtime_error("d");
		}

		// 変換行列の登録
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

	printf("シーンの初期化に成功\n");
	return true;
}

void SquareRenderer::Update(RenderProperty tran)
{
	auto currentIndex = m_core->m_frameIndex; // 現在のフレーム番号を取得
	auto currentTransform = m_constantBuffer[currentIndex]->GetPtr<Transform>(); // 現在のフレーム番号に対応する定数バッファを取得
	//currentTransform->World = DirectX::XMMatrixRotationY(tran.Position.y); // Y軸で回転させる
	currentTransform->World = XMMatrixScaling(tran.Scale.x, tran.Scale.y, tran.Scale.z) *
		XMMatrixRotationX(tran.Rotation.x) *
		XMMatrixRotationY(tran.Rotation.y) *
		XMMatrixRotationZ(tran.Rotation.z) *
		XMMatrixTranslation(tran.Position.x, tran.Position.y, tran.Position.z);
}

void SquareRenderer::Draw()
{
	auto currentIndex = m_core->m_frameIndex; // 現在のフレーム番号を取得する
	auto commandList = m_core->GetCommandList(); // コマンドリスト
	auto vbView = m_vertexBuffer->View();
	auto ibView = m_indexBuffer->View(); // インデックスバッファビュー

	commandList->SetGraphicsRootSignature(m_rootSignature->Get());
	commandList->SetPipelineState(m_pipelineState->Get());
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer[currentIndex]->GetAddress());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView); // インデックスバッファをセットする

	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 6個のインデックスで描画する（三角形の時と関数名が違うので注意）
}
