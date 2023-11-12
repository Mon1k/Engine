#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>
using namespace std;

#include "../shaders/AbstractShader.h"
#include "../lightclass.h"
#include "../Options.h"

class ShadowShader : public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX  f4x4WorldViewProjection;      // World * View * Projection matrix  
		D3DXMATRIX  f4x4WorldViewProjLight;       // World * ViewLight * Projection Light matrix  
		D3DXVECTOR4 vShadowMapDimensions;
		D3DXVECTOR4 vLightDir;
		float       fSunWidth;
		float		fDepthPower;
		float       f3Pad[2];
	};

public:
	ShadowShader();
	~ShadowShader();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, LightClass*);

	void addLights(std::vector<LightClass*>);
	LightClass* getLight(int index)
	{
		return m_lights[index];
	}

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, LightClass*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, bool);
	void RenderShader(ID3D11DeviceContext*, int);
	void RenderShaderDepth(ID3D11DeviceContext*, int);

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	
	

private:
	ID3D11VertexShader* m_vertexShader2;
	ID3D11InputLayout* m_layout;
	
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11SamplerState* m_sampleStateLinear;

	ID3D11Buffer* m_matrixBuffer;

	std::vector<LightClass*> m_lights;
};
