#pragma once
#include <map>
#include <string>

#include "Texture2D.h"

class Texture2DCache
{
private:
	inline static std::map<std::wstring, Texture2D*> m_cache = std::map<std::wstring, Texture2D*>();
public:
	static void Register(std::wstring path, Texture2D*& texture);
	static Texture2D* Find(std::wstring path);
	static void Clear();
};

