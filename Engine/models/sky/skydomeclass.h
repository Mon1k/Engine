#ifndef _SKYDOMECLASS_H_
#define _SKYDOMECLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <string>

#include "../Model.h"
#include "../../textures/skydomeshaderclass.h"

class SkyDomeClass : public Model
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
	};

public:
	SkyDomeClass();
	SkyDomeClass(const SkyDomeClass&);
	~SkyDomeClass();

	bool Initialize(D3DClass*, std::string);
	void RenderBuffers(ID3D11DeviceContext*);
	void Render(CameraClass*);

	D3DXVECTOR4 GetApexColor();
	D3DXVECTOR4 GetCenterColor();

	void setApexColor(D3DXVECTOR4 color)
	{
		m_apexColor = color;
	}
	void setCenterColor(D3DXVECTOR4 color)
	{
		m_centerColor = color;
	}

private:
	bool InitializeBuffers(ID3D11Device*);

private:
	D3DXVECTOR4 m_apexColor, m_centerColor;

	SkyDomeShaderClass* m_shader;
};

#endif