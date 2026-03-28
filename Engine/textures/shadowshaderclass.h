#ifndef _SHADOWSHADERCLASS_H_
#define _SHADOWSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>

#include "../shaders/AbstractShader.h"
#include "../lightclass.h"
#include "../Options.h"

class ShadowShaderClass : public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView[LightClass::NUM_LIGHTS];
		D3DXMATRIX lightProjection;
	};

	struct LightBufferType
	{
		LightClass::ShaderLight light[LightClass::NUM_LIGHTS];
	};


public:
	ShadowShaderClass();
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		std::vector <ID3D11ShaderResourceView*> shadowTextures, std::vector<LightClass*> lights);

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, std::vector <ID3D11ShaderResourceView*> shadowTextures, std::vector<LightClass*> lights);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11SamplerState* m_sampleStateShadow;
	ID3D11SamplerState* m_sampleStateTexture;
	ID3D11SamplerState* m_SamplePointCmp;

	ID3D11InputLayout* m_layout;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};

#endif