#pragma once
#include <random>

class Random
{
private:
	inline static std::random_device m_rnd = std::random_device();
	inline static std::uniform_real_distribution<> m_rand = std::uniform_real_distribution<>(0, 1);
	inline static std::mt19937 m_mt = std::mt19937(m_rnd());
public:
	static float RangeF(float end);
	static float RangeF(float begin, float end);
};

