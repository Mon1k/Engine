#include "fpsclass.h"


FpsClass::FpsClass()
{
}


FpsClass::FpsClass(const FpsClass& other)
{
}


FpsClass::~FpsClass()
{
}

void FpsClass::Initialize()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();


	PDH_STATUS status;

	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	m_canReadCpu = true;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, NULL, &m_queryHandle);
	if (status != ERROR_SUCCESS) {
		m_canReadCpu = false;
	}

	// Set query object to poll all cpus in the system.
	status = PdhAddEnglishCounter(m_queryHandle, L"\\Processor(_Total)\\% Processor Time", NULL, &m_counterHandle);
	if (status != ERROR_SUCCESS) {
		m_canReadCpu = false;
	}
	PdhCollectQueryData(m_queryHandle);
	m_cpuUsage = 0;

	return;
}

void FpsClass::Frame()
{
	m_count++;
	if (timeGetTime() >= (m_startTime + 1000)) {
		m_fps = m_count;
		m_count = 0;
		m_startTime = timeGetTime();

		if (m_canReadCpu) {
			PDH_FMT_COUNTERVALUE value;
			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_DOUBLE, NULL, &value);
			m_cpuUsage = value.doubleValue;
		}
	}
}

void FpsClass::Shutdown()
{
	if (m_canReadCpu) {
		PdhCloseQuery(m_queryHandle);
	}

	return;
}

double FpsClass::GetCpuPercentage()
{
	if (m_canReadCpu) {
		return m_cpuUsage;
	} else {
		return -1;
	}
}


int FpsClass::GetFps()
{
	return m_fps;
}