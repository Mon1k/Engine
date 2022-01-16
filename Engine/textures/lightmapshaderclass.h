#ifndef _LIGHTMAPSHADERCLASS_H_
#define _LIGHTMAPSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

#include "../shaders/AbstractShader.h"

class LightMapShaderClass: public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

public:
	LightMapShaderClass();
	LightMapShaderClass(const LightMapShaderClass&);
	~LightMapShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3);

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif