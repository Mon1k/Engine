#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "models/AbstractModel.h"
#include "models/CompositeModel.h"
#include "textureclass.h"
#include "textures/texturearrayclass.h"
#include "lightclass.h"
#include "models/bbox.h"

class ModelClass : public AbstractModel
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

	virtual bool Initialize(D3DClass*, char*, std::vector<std::string>);
	virtual void Shutdown();
	virtual void Render(CameraClass*);
	virtual void Render();
	virtual bool InitializeBuffers();
	
	virtual void CalcMinMax();
	void CalcMinMaxSubsets();

	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetTexture(int);
	TextureArrayClass* GetTextureClass()
	{
		return m_TextureArray;
	}

	ID3D11ShaderResourceView** GetTextureArray();
	bool addTexture(std::string);
	bool LoadTextures(std::string);
	bool LoadTexturesArray(std::vector<std::string>);

	std::string getPath()
	{
		return m_path;
	}

	void ReleaseTexture();
	void ReleaseModel();

	virtual D3DXMATRIX GetWorldMatrix();	

	bool LoadModel(char* filename);
	
	void addLights(std::vector<LightClass*>);
	
	LightClass* getLight(int index)
	{
		return m_lights[index];
	}

	std::vector<LightClass*> getLights()
	{
		return m_lights;
	}

	virtual void frame(CameraClass*, float);

	void addSubset(ModelClass* subset);
	CompositeModel* getSubset()
	{
		return m_subsets;
	}

	virtual int GetTriangleCount()
	{
		int count = m_vertexCount / 3;
		if (m_subsets) {
			for (int i = 0; i < m_subsets->getChilds().size(); i++) {
				count += m_subsets->getChilds()[i]->GetTriangleCount();
			}
		}

		return count;
	}

	virtual void SetPosition(D3DXVECTOR3 _position)
	{
		AbstractModel::SetPosition(_position);
		if (m_subsets) {
			m_subsets->SetPosition(_position);
			CalcMinMaxSubsets();
		}
	}

	virtual void SetScale(D3DXVECTOR3 _scale)
	{
		AbstractModel::SetScale(_scale);
		if (m_subsets) {
			m_subsets->SetScale(_scale);
			CalcMinMaxSubsets();
		}
	}

	virtual void SetRotation2(D3DXVECTOR3 _rotation)
	{
		AbstractModel::SetRotation(_rotation);
		if (m_subsets) {
			m_subsets->SetRotation(_rotation);
		}
	}

	virtual void setAlpha(bool alpha)
	{
		AbstractModel::setAlpha(alpha);

		if (m_subsets) {
			m_subsets->setAlpha(alpha);
		}
	}

	virtual void setShadow(bool shadow)
	{
		AbstractModel::setShadow(shadow);

		if (m_subsets) {
			m_subsets->setShadow(shadow);
		}
	}

	virtual ID3D11Buffer* getVertexBuffer()
	{
		return m_vertexBuffer;
	}

	virtual ID3D11Buffer* getIndexBuffer()
	{
		return m_indexBuffer;
	}

	BBox* showBBox();
	void hideBBox();
	void refreshBBox()
	{
		hideBBox();
		showBBox();
	}
	BBox* getBBox()
	{
		return m_BBox;
	}

protected:
	virtual void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);

protected:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	
	CompositeModel* m_subsets;
	TextureArrayClass* m_TextureArray;
	std::string m_path;
	std::vector<LightClass*> m_lights;

	BBox* m_BBox;
};

#endif