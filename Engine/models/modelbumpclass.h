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

	bool Initialize(D3DClass*, char*, std::vector<std::string>);

protected:
	bool InitializeBuffers(ID3D11Device*);

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
};

#endif