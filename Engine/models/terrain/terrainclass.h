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

	bool Initialize(D3DClass*, FrustumClass*, char*, std::string, ::string);
	virtual void Shutdown();
	virtual void Render(CameraClass*);

	float getTextureRepeat()
	{
		return m_repeat ? m_repeat : m_terrainWidth / 16;
	}
	void setTextureRepeat(float repeat)
	{
		m_repeat = repeat;
	}
	void addDetailTexture(std::string texture)
	{
		addTexture(texture);
		isDetailTexture = m_TextureArray->getTextures().size() - 1;
	}
	void addBumpTexture(std::string texture)
	{
		addTexture(texture);
		isBumpTexture = m_TextureArray->getTextures().size() - 1;
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

	int GetTtriangleCount() {
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
	int isDetailTexture, isBumpTexture;
	D3DXVECTOR3 m_scaleNormal;

	HeightMapType* m_heightMap;
	HeightMapType* m_TerrainModel;

	VertexType* m_vertices;
	QuadTreeClass* m_quadTree;
};

#endif