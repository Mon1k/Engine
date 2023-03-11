#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include "../../modelclass.h"

class TerrainClass : public ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	virtual void Render(CameraClass*);
	virtual void Render();
	bool Initialize(D3DClass*);

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	int m_terrainWidth, m_terrainHeight;
};

#endif