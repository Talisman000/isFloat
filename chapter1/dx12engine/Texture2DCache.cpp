#include "Texture2DCache.h"


void Texture2DCache::Register(std::wstring path, Texture2D*& texture)
{
	const auto it = m_cache.find(path);
	if (it != m_cache.end()) return;
	m_cache[path] = texture;
	OutputDebugString(L"[Info] Texture2DCache registered : ");
	OutputDebugString(path.c_str());
	OutputDebugString(L"\n");
}

Texture2D* Texture2DCache::Find(const std::wstring path)
{
	const auto it = m_cache.find(path);
	if (it == m_cache.end())
	{
		return nullptr;
	}
	return it->second;
}

void Texture2DCache::Clear()
{
	for (const auto& texture : m_cache)
	{
		delete texture.second;
	}
}
