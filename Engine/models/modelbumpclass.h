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

	virtual int GetIndexCount() {
		return m_indexCount;
	}
	virtual int GetTtriangleCount() {
		return m_vertexCount / 3;
	}

	bool Initialize(D3DClass*, char*, std::vector<std::wstring>);
	bool LoadModelDs(char*);

protected:
	bool InitializeBuffers(ID3D11Device*);

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;

	ModelType* m_model;
protected:
	int m_vertexCount, m_indexCount;
};

#endif