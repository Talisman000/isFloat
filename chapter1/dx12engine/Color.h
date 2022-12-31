#pragma once
#include <DirectXMath.h>

namespace Color
{
	constexpr DirectX::XMFLOAT4 red(1.0f, 0.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 green(0.0f, 1.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 blue(0.0f, 0.0f, 1.0f, 1.0);
	constexpr DirectX::XMFLOAT4 white(1.0f, 1.0f, 1.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 magenta(1.0f, 0.0f, 1.0f, 1.0f);
	constexpr DirectX::XMFLOAT4 cyan(0.0f, 1.0f, 1.0f, 1.0f);
}
