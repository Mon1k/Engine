#ifndef _SKYPLANECLASS_H_
#define _SKYPLANECLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>

#include "../../modelclass.h"
#include "../../textures/skyplaneshaderclass.h"

class SkyPlaneClass : public ModelClass
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
	SkyPlaneClass();
	SkyPlaneClass(const SkyPlaneClass&);
	~SkyPlaneClass();

	bool Initialize(D3DClass*, std::vector<std::string>);
	void RenderBuffers(ID3D11DeviceContext*);
	void Render(CameraClass*);
	void Shutdown();
	void frame(CameraClass* camera, float time);

	void setBrightness(float brightness) {
		m_brightness = brightness;
	}

	float GetBrightness();
	float GetTranslation();

private:
	bool InitializeSkyPlane(int, float, float, float, int);
	void ShutdownSkyPlane();

	bool InitializeBuffers(ID3D11Device*, int);

private:
	SkyPlaneType* m_skyPlane;

	float m_brightness, m_translation;
	float m_counter;

	SkyPlaneShaderClass* m_shader;
};

#endif