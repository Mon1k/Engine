#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "models/AbstractModel.h"
#include "d3dclass.h"
#include "textureclass.h"
#include "textures/texturearrayclass.h"

#include "models/loader/DsLoader.h"
#include "models/loader/ObjLoader.h"

class ModelClass: public AbstractModel
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

	D3DXVECTOR3 GetPosition() {
		return position;
	}
	D3DXVECTOR3 GetScale() {
		return scale;
	}
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();
	ID3D11ShaderResourceView** GetTextureArray(int);

	void ReleaseModel();
	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&);
	void SetPosition(D3DXVECTOR3);
	void SetScale(D3DXVECTOR3);
	D3DXMATRIX GetWorldMatrix();

	bool LoadModel(char* filename) {
		std::string string(filename);

		if (string.rfind(".ds") != std::string::npos) {
			DsLoader* loader = new DsLoader;
			return loader->load(filename, this);
		}
		else if (string.rfind(".obj") != std::string::npos) {
			ObjLoader* loader = new ObjLoader;
			return loader->load(filename, this);
		}

		return false;
	}

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
	
protected:
	D3DClass* m_D3D;

	D3DXVECTOR3 m_Min, m_Max;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;

	TextureClass* m_Texture; // @todo remove
	TextureArrayClass* m_TextureArray;
};

#endif