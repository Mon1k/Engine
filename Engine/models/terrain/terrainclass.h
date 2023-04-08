#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>

#include "../../modelclass.h"
#include "quadtreeclass.h"
#include "../../textures/terrainshaderclass.h"
#include "../../frustumclass.h"

class TerrainClass : public ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 texture;
		D3DXVECTOR3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
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

	bool Initialize(D3DClass*, FrustumClass*, char*, WCHAR*, WCHAR*);
	virtual void Shutdown();
	virtual void Render(CameraClass*);

	float getTextureRepeat() {
		return m_repeat ? m_repeat : m_terrainWidth / 16;
	}
	void setTextureRepeat(float repeat) {
		m_repeat = repeat;
	}

	int GetTtriangleCount() {
		return m_quadTree->GetDrawCount();
	}

	void CopyVertexArray(void*);
	QuadTreeClass* getQuadTree() {
		return m_quadTree;
	}

	virtual AbstractShader* getShader() {
		return m_shader;
	}

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void CalculateTextureCoordinates();
	void ShutdownHeightMap();
	void ShutdownBuffers();

	bool InitializeBuffers(ID3D11Device*);
private:
	int m_terrainWidth, m_terrainHeight;
	float m_repeat;

	TerrainShaderClass* m_shader;
	HeightMapType* m_heightMap;

	VertexType* m_vertices;
	QuadTreeClass* m_quadTree;
};

#endif