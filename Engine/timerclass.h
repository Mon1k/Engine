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
	float getDifferenceTime() {
		return m_differenceTime;
	};

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
	float m_differenceTime;
};

#endif