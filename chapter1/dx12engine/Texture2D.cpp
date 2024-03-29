#include "Texture2D.h"
#include "Texture2D.h"
#include <DirectXTex.h>
#include <stdexcept>

#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

// std::string(マルチバイト文字列)からstd::wstring(ワイド文字列)を得る。AssimpLoaderと同じものだけど、共用にするのがめんどくさかったので許してください
std::wstring GetWideString(const std::string& str)
{
	auto num1 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);

	std::wstring wstr;
	wstr.resize(num1);

	auto num2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str.c_str(), -1, &wstr[0], num1);

	assert(num1 == num2);
	return wstr;
}

// 拡張子を返す
std::wstring FileExtension(const std::wstring& path)
{
	auto idx = path.rfind(L'.');
	return path.substr(idx + 1, path.length() - idx - 1);
}

Texture2D::Texture2D(Core* core, std::string path)
{
	m_core = core;
	m_IsValid = Load(path);
}

Texture2D::Texture2D(Core* core, std::wstring path)
{
	m_core = core;
	m_IsValid = Load(path);
}

Texture2D::Texture2D(Core* core, ComPtr<ID3D12Resource> buffer)
{
	m_core = core;
	m_pResource = buffer;
	m_IsValid = m_pResource != nullptr;
}

bool Texture2D::Load(std::string& path)
{
	auto wpath = GetWideString(path);
	return Load(wpath);
}

bool Texture2D::Load(std::wstring& path)
{
	//WICテクスチャのロード
	TexMetadata meta = {};
	ScratchImage scratch = {};
	auto ext = FileExtension(path);

	HRESULT hr = S_FALSE;
	if (ext == L"png") // pngの時はWICFileを使う
	{
		LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &meta, scratch);
	}
	else if (ext == L"tga") // tgaの時はTGAFileを使う
	{
		hr = LoadFromTGAFile(path.c_str(), &meta, scratch);
	}

	if (FAILED(hr))
	{
		return false;
	}

	auto img = scratch.GetImage(0, 0, 0);
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(meta.format,
		meta.width,
		meta.height,
		static_cast<UINT16>(meta.arraySize),
		static_cast<UINT16>(meta.mipLevels));

	// リソースを生成
	hr = m_core->GetDevice()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(m_pResource.ReleaseAndGetAddressOf())
	);

	if (FAILED(hr))
	{
		throw std::runtime_error("a");
	}

	hr = m_pResource->WriteToSubresource(0,
		nullptr, //全領域へコピー
		img->pixels, //元データアドレス
		static_cast<UINT>(img->rowPitch), //1ラインサイズ
		static_cast<UINT>(img->slicePitch) //全サイズ
	);
	if (FAILED(hr))
	{
		throw std::runtime_error("b");
	}

	return true;
}

Texture2D* Texture2D::Get(Core* core, std::string path)
{
	auto wpath = GetWideString(path);
	return Get(core, wpath);
}

Texture2D* Texture2D::Get(Core* core, std::wstring path)
{
	auto tex = new Texture2D(core, path);
	if (!tex->IsValid())
	{
		return GetWhite(core); // 読み込みに失敗した時は白単色テクスチャを返す
	}
	return tex;
}

Texture2D* Texture2D::GetWhite(Core* core)
{
	auto buff = GetDefaultResource(core, 4, 4);

	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);

	auto hr = buff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	if (FAILED(hr))
	{
		return nullptr;
	}


	return new Texture2D(core, buff);
}

ComPtr<ID3D12Resource> Texture2D::GetDefaultResource(Core* core, size_t width, size_t height)
{
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	ComPtr<ID3D12Resource> buff = nullptr;
	auto result = core->GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, //特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(buff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		assert(SUCCEEDED(result));
		return nullptr;
	}
	return buff;
}

bool Texture2D::IsValid()
{
	return m_IsValid;
}

ComPtr<ID3D12Resource> Texture2D::Resource()
{
	return m_pResource;
}

D3D12_SHADER_RESOURCE_VIEW_DESC Texture2D::ViewDesc()
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	auto res = m_pResource;
	desc.Format = m_pResource->GetDesc().Format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; //2Dテクスチャ
	desc.Texture2D.MipLevels = 1; //ミップマップは使用しないので1
	return desc;
}
