#include "timerclass.h"


TimerClass::TimerClass()
{
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0) {
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_differenceTime = currentTime - m_startTime;
	m_frameTime = (float)m_differenceTime / m_ticksPerMs;
	m_startTime = currentTime;
}

float TimerClass::GetTime()
{
	return m_frameTime;
}