#pragma once
#include "d3dx12.h"
#include <DirectXMath.h>
#include "ComPtr.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT4 Color;

	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 5;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) ShaderProperty {
	DirectX::XMMATRIX World; // ワールド行列
	DirectX::XMMATRIX View; // ビュー行列
	DirectX::XMMATRIX Proj; // 投影行列
};


struct Transform
{
	DirectX::XMFLOAT3 Position = { 0,0,0 };
	DirectX::XMFLOAT3 Rotation = { 0,0,0 };
	DirectX::XMFLOAT3 Scale = { 1,1,1 };
};


