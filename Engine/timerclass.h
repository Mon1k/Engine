#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


#include <windows.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();
	INT64 getDifferenceTime()
	{
		return m_differenceTime;
	}

	INT64 getFrequency()
	{
		return m_frequency;
	}

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
	INT64 m_differenceTime;
};

#endif