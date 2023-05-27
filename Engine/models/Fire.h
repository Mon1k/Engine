#ifndef _FIRE_H_
#define _FIRE_H_

#include "../modelclass.h"
#include "../textures/fireshaderclass.h"

class Fire : public ModelClass
{
public:
	virtual bool Initialize(D3DClass*, char*, std::vector<std::string>);
	virtual void Render(CameraClass*);
	void Shutdown();
	void setFrameTime(float frameTime) {
		m_frameTime = frameTime;
	}
	float getFrameTime() {
		return m_frameTime;
	}

protected:
	FireShaderClass* m_FireShader;

	D3DXVECTOR3 m_scrollSpeeds, m_scales;
	D3DXVECTOR2 m_distortion1, m_distortion2, m_distortion3;
	float m_distortionScale, m_distortionBias;
	float m_frameTime;
};

#endif