#ifndef _WATERNORE_H_
#define _WATERNORE_H_

#include "Reflection.h"

#include "../textures/refractionshaderclass.h"
#include "../textures/watershaderclass.h"
#include "../lightshaderclass.h"

class WaterNode: public Reflection
{
public:
	virtual bool Initialize(D3DClass*, char*, std::vector<std::wstring>);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	void RenderRefractionToTexture(CameraClass*);
	void RenderReflectionToTexture(CameraClass*);
	void Shutdown();
	void addRefractionTarget(ModelClass* model)
	{
		this->m_RefractionModel = model;
	}

	void incrementTranslation()
	{
		m_waterTranslation += 0.005f;
		if (m_waterTranslation > 1.0f) {
			m_waterTranslation -= 1.0f;
		}
	}
	void setWaterHeight(float height)
	{
		m_waterHeight = height;
	}

protected:
	RenderTextureClass* m_RefractionTexture;
	RefractionShaderClass* m_RefractionShader;
	WaterShaderClass* m_WaterShader;
	LightShaderClass* m_LightShaderWater;
	ModelClass* m_RefractionModel;

	float m_waterHeight;
	float m_waterTranslation;
	float m_reflectRefractScale;
	float m_specularShininess;
	D3DXVECTOR2 m_normalMapTiling;
	D3DXVECTOR4 m_refractionTint;
};

#endif