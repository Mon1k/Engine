#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>

#include <iostream>
#include <fstream>
#include <vector>
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

	bool Initialize(D3DClass*, char*, std::vector<std::wstring>);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	virtual int GetIndexCount() {
		return m_indexCount;
	}
	virtual int GetTtriangleCount() {
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
	ID3D11ShaderResourceView** GetTextureArray(int);

	bool LoadModel(char*);
	virtual bool LoadModelDs(char*);
	virtual bool LoadModelObj(char*);
	void ReleaseModel();
	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&);
	void SetPosition(D3DXVECTOR3);
	void SetScale(D3DXVECTOR3);
	D3DXMATRIX GetWorldMatrix();

protected:
	void CalcMinMax();
	virtual bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, std::wstring);
	bool LoadTextures(ID3D11Device*, std::wstring, std::wstring);
	bool LoadTexturesArray(ID3D11Device*, std::vector<std::wstring>);
	void ReleaseTexture();

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	
	ModelType* m_model;

protected:
	int m_vertexCount, m_indexCount;
	D3DClass* m_D3D;

	D3DXVECTOR3 m_Min, m_Max;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;

	TextureClass* m_Texture;
	TextureArrayClass* m_TextureArray;
};

#endif