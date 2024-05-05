#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "../modelclass.h"
#include "../colorshaderclass.h"

class GridClass: public ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR4 color;
	};

public:
	GridClass();
	GridClass(const GridClass&);
	~GridClass();

	bool Initialize(D3DClass* d3dClass, int width, int height);

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void Render(CameraClass*);
	
private:
	int m_terrainWidth, m_terrainHeight;
	ColorShaderClass* m_shader;
};