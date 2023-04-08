#ifndef _QUADTREECLASS_H_
#define _QUADTREECLASS_H_

const int MAX_TRIANGLES = 10000;

#include <fstream>
#include <vector>
using namespace std;

#include "../../d3dclass.h"
#include "../../textures/terrainshaderclass.h"
#include "../../frustumclass.h"


class QuadTreeClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 texture;
		D3DXVECTOR3 normal;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer* vertexBuffer, *indexBuffer;
		VectorType* vertexArray;
		NodeType* nodes[4];
		NodeType* parent;
		vector<int> indexes;
	};

public:
	QuadTreeClass();
	QuadTreeClass(const QuadTreeClass&);
	~QuadTreeClass();

	bool Initialize(void*, int, D3DClass*, FrustumClass*);

	void Shutdown();
	void Render(TerrainShaderClass*);

	int GetDrawCount();
	bool GetHeightAtPosition(float, float, float&);


private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float);
	int CountTriangles(NodeType*, float, float, float);
	bool isCountTriangles(NodeType*, float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void FindNode(NodeType*, float, float, float&);
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, TerrainShaderClass*);

private:
	int m_triangleCount, m_drawCount;
	float m_width;

	VertexType* m_vertexList;
	NodeType* m_parentNode;
	FrustumClass* m_frustum;

	D3DClass* m_D3D;
};

#endif