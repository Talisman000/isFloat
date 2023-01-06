#include "Time.h"

#include <atlstr.h>
#include <chrono>
#include <Windows.h>


void Time::Init()
{
	const auto now = std::chrono::system_clock::now();
	m_currentTime = now;
	m_prevTime = now;
}

void Time::SetCurrent()
{
	m_prevTime = m_currentTime;
	m_currentTime = std::chrono::system_clock::now();
}

float Time::DeltaTime()
{
	const auto deltaSec = std::chrono::duration_cast<std::chrono::microseconds>(m_currentTime - m_prevTime).count();
	const float delta = static_cast<float>(deltaSec) * 0.000001f;
//#if _DEBUG
//	CString cs;
//	cs.Format(_T("[delta] %2.2f\n"), delta);
//	OutputDebugString(cs);
//#endif
	return delta;
}


