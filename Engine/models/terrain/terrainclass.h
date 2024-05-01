#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>

#include "../Model.h"
#include "quadtreeclass.h"
#include "../../textures/terrainshaderclass.h"
#include "../../frustumclass.h"

class TerrainClass : public Model
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
		D3DXVECTOR2 texture2;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float tu2, tv2;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(D3DClass*, FrustumClass*, char*, std::string, std::string);
	virtual void Shutdown();
	virtual void Render(CameraClass*);
	virtual void Render(CameraClass*, ID3D11ShaderResourceView*);

	float getTextureRepeat()
	{
		return m_repeat ? m_repeat : m_terrainWidth / 16;
	}
	void setTextureRepeat(float repeat)
	{
		m_repeat = repeat;
	}
	void addTextureLayer(std::string texture, std::string normalTexture)
	{
		addTexture(texture);
		addTexture(normalTexture);
	}
	void addTextureAlpha(std::string texture)
	{
		addTexture(texture);
	}

	int GetTriangleCount()
	{
		return m_quadTree->GetDrawCount();
	}

	void CopyVertexArray(void*);
	QuadTreeClass* getQuadTree()
	{
		return m_quadTree;
	}

	void setScaleNormal(D3DXVECTOR3 scale)
	{
		m_scaleNormal = scale;
	}
	D3DXVECTOR3 getScaleNormal()
	{
		return m_scaleNormal;
	}

	int getWidth()
	{
		return m_terrainWidth;
	}

	int getHeight()
	{
		return m_terrainHeight;
	}

	D3DXVECTOR3 getOrigScale()
	{
		return m_origScale;
	}

private:
	bool LoadHeightMap(char*);
	bool CalculateNormals();
	void CalculateTextureCoordinates();
	void ShutdownHeightMap();
	void ShutdownBuffers();

	bool InitializeBuffers(ID3D11Device*);

	void CalculateTerrainVectors();
	bool BuildTerrainModel();
	void CalculateTangentBinormal(HeightMapType, HeightMapType, HeightMapType, VectorType&, VectorType&);
private:
	int m_terrainWidth, m_terrainHeight;
	float m_repeat;
	D3DXVECTOR3 m_scaleNormal;
	D3DXVECTOR3 m_origScale;

	HeightMapType* m_heightMap;
	HeightMapType* m_TerrainModel;

	VertexType* m_vertices;
	QuadTreeClass* m_quadTree;
};

#endif