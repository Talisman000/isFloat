#pragma once
#include "ComPtr.h"
#include <string>

#include "Core.h"
#include "d3dx12.h"

class DescriptorHeap;
class DescriptorHandle;

class Texture2D
{
public:
	static Texture2D* Get(Core* core, std::string path); // stringで受け取ったパスからテクスチャを読み込む
	static Texture2D* Get(Core* core, std::wstring path); // wstringで受け取ったパスからテクスチャを読み込む
	static Texture2D* GetWhite(Core* core); // 白の単色テクスチャを生成する
	bool IsValid(); // 正常に読み込まれているかどうかを返す

	ComPtr<ID3D12Resource> Resource(); // リソースを返す
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc(); // シェーダーリソースビューの設定を返す

private:
	Core* m_core;
	bool m_IsValid; // 正常に読み込まれているか
	Texture2D(Core* core, std::string path);
	Texture2D(Core* core, std::wstring path);
	Texture2D(Core* core, ComPtr<ID3D12Resource> buffer);
	ComPtr<ID3D12Resource> m_pResource; // リソース
	bool Load(std::string& path);
	bool Load(std::wstring& path);

	static ComPtr<ID3D12Resource> GetDefaultResource(Core* core, size_t width, size_t height);

	Texture2D(const Texture2D&) = delete;
	void operator = (const Texture2D&) = delete;
};
