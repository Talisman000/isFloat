#pragma once
#include <d3d12.h>
#include "ComPtr.h"
class VertexBuffer
{
public:
	VertexBuffer(ID3D12Device* device, size_t size, size_t stride, const void* pInitData); // コンストラクタでバッファを生成
	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW View() const; // 頂点バッファビューを取得
	[[nodiscard]] bool IsValid() const; // バッファの生成に成功したかを取得

private:
	bool m_IsValid = false; // バッファの生成に成功したかを取得
	ComPtr<ID3D12Resource> m_pBuffer = nullptr; // バッファ
	D3D12_VERTEX_BUFFER_VIEW m_View = {}; // 頂点バッファビュー
	VertexBuffer(const VertexBuffer&) = delete;
	void operator = (const VertexBuffer&) = delete;
};

