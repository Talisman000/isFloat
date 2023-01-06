#include "Random.h"

float Random::RangeF(const float end)
{
	return RangeF(0, end);
}

float Random::RangeF(float begin, float end)
{
	const auto range = end - begin;
	const auto rand = static_cast<float>(m_rand(m_mt));
	return rand * range + begin;
}
