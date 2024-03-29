#pragma once
#include "ComPtr.h"
#include "d3dx12.h"


class RootSignature
{
public:
	RootSignature(const ComPtr<ID3D12Device>& device); // コンストラクタでルートシグネチャを生成
	bool IsValid(); // ルートシグネチャの生成に成功したかどうかを返す
	ComPtr<ID3D12RootSignature> Get() const; // ルートシグネチャを返す

private:
	bool m_IsValid = false; // ルートシグネチャの生成に成功したかどうか
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr; // ルートシグネチャ
};

