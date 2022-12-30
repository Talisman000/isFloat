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

	m_rootSignature = new RootSignature(device);
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

void TriangleRenderer::Update(RenderProperty tran)
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

void TriangleRenderer::Draw()
{
	auto currentIndex = m_core->m_frameIndex; // 現在のフレーム番号を取得する
	auto commandList = m_core->GetCommandList(); // コマンドリスト
	auto vbView = m_vertexBuffer->View(); // 頂点バッファビュー

	commandList->SetGraphicsRootSignature(m_rootSignature->Get()); // ルートシグネチャをセット
	commandList->SetPipelineState(m_pipelineState->Get()); // パイプラインステートをセット
	commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer[currentIndex]->GetAddress()); // 定数バッファをセット

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 三角形を描画する設定にする
	commandList->IASetVertexBuffers(0, 1, &vbView); // 頂点バッファをスロット0番を使って1個だけ設定する

	commandList->DrawInstanced(3, 1, 0, 0); // 3個の頂点を描画する
}
