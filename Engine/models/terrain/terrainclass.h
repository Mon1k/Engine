#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>

#include "../../modelclass.h"
#include "../../textures/terrainshaderclass.h"

class TerrainClass : public ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	virtual void Render(CameraClass*);
	virtual void Render();
	bool Initialize(D3DClass*, char*);

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	int m_terrainWidth, m_terrainHeight;

	HeightMapType* m_heightMap;
	TerrainShaderClass* m_shader;
};

#endif