#ifndef _SKYDOMECLASS_H_
#define _SKYDOMECLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <string>

#include "../../modelclass.h"
#include "../../textures/skydomeshaderclass.h"

class SkyDomeClass : public ModelClass
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

private:
	bool InitializeBuffers(ID3D11Device*);

private:
	D3DXVECTOR4 m_apexColor, m_centerColor;

	SkyDomeShaderClass* m_shader;
};

#endif