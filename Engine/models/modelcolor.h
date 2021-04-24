#ifndef _MODELCOLORCLASS_H_
#define _MODELCOLORCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

class ModelColorClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	ModelColorClass();
	ModelColorClass(const ModelColorClass&);
	~ModelColorClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void SetColor(D3DXVECTOR4 color) {
		this->color = color;
	};
	void SetPoint(D3DXVECTOR3 point1, D3DXVECTOR3 point2) {
		this->point1 = point1;
		this->point2 = point2;
	};

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	D3DXVECTOR4 color;
	D3DXVECTOR3 point1, point2;
};

#endif