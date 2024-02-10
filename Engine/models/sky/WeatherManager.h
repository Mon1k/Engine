#pragma once

#include "../../render/passes/VolumetricClouds.h"

class WeatherManager
{
public:
	WeatherManager(VolumetricClouds* clouds)
	{
		setClouds(clouds);
	}

	void setClouds(VolumetricClouds* clouds)
	{
		m_clouds = clouds;
		m_startParams = m_clouds->getParams();
		m_currentParams = m_startParams;
	}

	VolumetricClouds::CloudParameters getParams()
	{
		return m_currentParams;
	}

	void setNextTarget()
	{
		m_targetParams = m_startParams;
		m_targetParams.global_density = 0.75f;
	}

	void frame(float time)
	{
		m_lastTime += time;

		if (m_powerChange && m_lastTime >= 1000) {
			m_lastTime = 0;

			if (m_dir > 0.0f && m_currentParams.global_density > m_targetParams.global_density) {
				m_dir = -m_dir;
				m_targetParams.global_density = 1.0f - m_targetParams.global_density;
			} else if (m_dir < 0.0f && m_currentParams.global_density < m_targetParams.global_density) {
				m_dir = -m_dir;
				m_targetParams.global_density = 1.0f - m_targetParams.global_density;
			}
			m_currentParams.global_density += m_dir * time * m_powerChange;
			m_clouds->setParams(m_currentParams);
		}

		
	}

protected:
	VolumetricClouds* m_clouds;


	float m_powerChange = 0.0005f;
	float m_dir = 1.0f;
	float m_lastTime = 0.0f;

	VolumetricClouds::CloudParameters m_startParams;
	VolumetricClouds::CloudParameters m_currentParams;
	VolumetricClouds::CloudParameters m_targetParams;

};