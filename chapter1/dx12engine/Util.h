#pragma once
#include <chrono>

class Time
{
	inline static std::chrono::system_clock::time_point m_prevTime = std::chrono::system_clock::now();
	inline static std::chrono::system_clock::time_point m_currentTime = std::chrono::system_clock::now();
public:
	static void Init();
	static void SetCurrent();
	static float DeltaTime();
};


