#ifndef _QUADTREECLASS_H_
#define _QUADTREECLASS_H_

const int MAX_TRIANGLES = 100000;

#include "../../d3dclass.h"
#include "../../textures/terrainshaderclass.h"
#include "../../frustumclass.h"


class QuadTreeClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer, *indexBuffer;
		NodeType* nodes[4];
	};

public:
	QuadTreeClass();
	QuadTreeClass(const QuadTreeClass&);
	~QuadTreeClass();

	bool Initialize(void*, int, D3DClass*, FrustumClass*);

	void Shutdown();
	void Render(TerrainShaderClass*);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, TerrainShaderClass*);

private:
	int m_triangleCount, m_drawCount;

	VertexType* m_vertexList;
	NodeType* m_parentNode;
	FrustumClass* m_frustum;

	D3DClass* m_D3D;
};

#endif