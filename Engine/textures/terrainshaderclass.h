#ifndef _TERRAINSHADERCLASS_H_
#define _TERRAINSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include "../shaders/AbstractShader.h"
#include "texturearrayclass.h"
#include "../lightclass.h"

class TerrainShaderClass : public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float lightIntensity;
		float lightDetailIntensity;
		float distanceIntensity;
		float countLayers;
		float shadowSize;
	};

public:
	TerrainShaderClass();
	TerrainShaderClass(const TerrainShaderClass&);
	~TerrainShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, LightClass*, TextureArrayClass*, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, LightClass*, TextureArrayClass*, ID3D11ShaderResourceView*);

	void setLightDetailIntensity(float intensity) {
		m_lightDetailIntensity = intensity;
	}
	void setDistnaceIntensity(float distance) {
		m_distanceIntensity = distance;
	}

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

private:
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11SamplerState* m_SamplePointCmp;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	float m_lightDetailIntensity;
	float m_distanceIntensity;
};

#endif