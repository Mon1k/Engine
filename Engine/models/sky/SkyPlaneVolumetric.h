#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "../../modelclass.h"
#include "../../render/passes/VolumetricClouds.h"
#include "../../lightshaderclass.h"

class SkyPlaneVolumetric : public ModelClass
{
private:
	struct SkyPlaneType
	{
		float x, y, z;
		float tu, tv;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	SkyPlaneVolumetric();
	~SkyPlaneVolumetric();

	bool Initialize(D3DClass*);
	void RenderBuffers(ID3D11DeviceContext*);
	void Render(CameraClass*);
	void Shutdown();
	void frame(CameraClass* camera, float time);

	void setBrightness(float brightness) {
		m_brightness = brightness;
	}

	float GetBrightness();
	float GetTranslation();

	VolumetricClouds* getShader()
	{
		return m_shader;
	}

private:
	bool InitializeSkyPlane(int, float, float, float, int);
	void ShutdownSkyPlane();

	bool InitializeBuffers(ID3D11Device*, int);

private:
	SkyPlaneType* m_skyPlane;

	float m_brightness, m_translation;
	float m_counter;

	VolumetricClouds* m_shader;
	LightShaderClass* m_shaderlight;
};