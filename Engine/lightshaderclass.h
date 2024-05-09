#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>

#include "shaders/AbstractShader.h"
#include "lightclass.h"
#include "textures/texturearrayclass.h"

class LightShaderClass: public AbstractShader
{
public:
	static const int NUM_LIGHTS = 4;

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct PointLight
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float lightIntencity;
		float specularPower;
		D3DXVECTOR4 specularColor;
	};

	struct LightBufferType
	{
		PointLight light[NUM_LIGHTS];
	};

	struct CameraBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding;
		D3DXVECTOR4 lightPosition[NUM_LIGHTS];
	};

public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, TextureArrayClass*, D3DXVECTOR3);
	
	void addLights(std::vector<LightClass*>);
	LightClass* getLight(int index)
	{
		return m_lights[index];
	}
	void clearLights()
	{
		m_lights.clear();
	}

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, TextureArrayClass*, D3DXVECTOR3);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightBuffer;

	std::vector<LightClass*> m_lights;
};

#endif