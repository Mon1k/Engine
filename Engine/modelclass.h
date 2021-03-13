#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "d3dclass.h"
#include "textureclass.h"
#include "textures/texturearrayclass.h"

class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(D3DClass*, char*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

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
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();

	bool LoadModel(char*);
	bool LoadModelDs(char*);
	bool LoadModelObj(char*);
	void ReleaseModel();
	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&);
	void SetPosition(D3DXVECTOR3);
	void SetScale(D3DXVECTOR3);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTexture();

private:
	D3DClass* m_D3D;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	D3DXVECTOR3 m_Min, m_Max;
	
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;

	TextureClass* m_Texture;
	TextureArrayClass* m_TextureArray;
	ModelType* m_model;
};

#endif