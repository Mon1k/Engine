#ifndef _FPSCLASS_H_
#define _FPSCLASS_H_


#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")


#include <windows.h>
#include <mmsystem.h>
#include <Pdh.h>
#include <PdhMsg.h>



class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();

	void Initialize();
	void Frame();
	void Shutdown();
	
	int GetFps();
	double GetCpuPercentage();


private:
	int m_fps, m_count;
	unsigned long m_startTime;

	bool m_canReadCpu;
	PDH_HQUERY  m_queryHandle;
	PDH_HCOUNTER  m_counterHandle;
	double m_cpuUsage;
};

#endif