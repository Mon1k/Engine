#ifndef _MODELBUMPCLASS_H_
#define _MODELBUMPCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

#include "../modelclass.h"

class ModelBumpClass: public ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
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
	ModelBumpClass();

	bool Initialize(D3DClass*, char*, std::vector<std::wstring>);
	void Render(ID3D11DeviceContext*);

	bool LoadModel(char*);
	bool LoadModelDs(char*);

	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&);
	void SetPosition(D3DXVECTOR3);
	void SetScale(D3DXVECTOR3);
	D3DXMATRIX GetWorldMatrix();
	int GetIndexCount();
	int GetTtriangleCount() {
		return m_vertexCount / 3;
	}
	D3DXVECTOR3 GetPosition() {
		return position;
	}
	D3DXVECTOR3 GetScale() {
		return scale;
	}

protected:
	bool InitializeBuffers(ID3D11Device*);

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

protected:
	void RenderBuffers(ID3D11DeviceContext*);

protected:
	D3DClass* m_D3D;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	D3DXVECTOR3 m_Min, m_Max;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;

	ModelType* m_model;
};

#endif