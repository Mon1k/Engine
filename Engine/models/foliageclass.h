#ifndef _FOLIAGECLASS_H_
#define _FOLIAGECLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <time.h>
#include <string>

#include "../modelclass.h"
#include "../textures/foliageshaderclass.h"
#include "terrain/terrainclass.h"

class FoliageClass : public ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	struct FoliageType
	{
		float x, y, z;
		float r, g, b;
	};

	struct InstanceType
	{
		D3DXMATRIX matrix;
		D3DXVECTOR3 color;
	};

public:
	FoliageClass();
	FoliageClass(const FoliageClass&);
	~FoliageClass();

	bool Initialize(D3DClass*, std::string, int);
	void Shutdown();
	void Render(CameraClass*);
	virtual void frame(CameraClass*, float);

	int GetInstanceCount();

	bool GeneratePositions(D3DXVECTOR3, D3DXVECTOR3);
	bool GeneratePositionsFromTerrain(D3DXVECTOR3, D3DXVECTOR3, TerrainClass*);
	bool GeneratePositionsFromTerrainWithMap(TerrainClass*, std::string);

	void setWindPower(float power)
	{
		m_windPower = power;
	}

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	FoliageShaderClass* m_shader;
	FoliageType* m_foliageArray;
	InstanceType* m_Instances;

	ID3D11Buffer* m_instanceBuffer;
	int m_instanceCount;

	int m_foliageCount;
	float m_windRotation;
	float m_windPower;
	float m_counter;
	int m_windDirection;
};

#endif