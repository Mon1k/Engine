#ifndef _PARTICLESHADERCLASS_H_
#define _PARTICLESHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include "../shaders/AbstractShader.h"

class ParticleShaderClass : public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct TransparentBufferType
	{
		float blendAmount;
		D3DXVECTOR3 padding;
	};

public:
	ParticleShaderClass();
	ParticleShaderClass(const ParticleShaderClass&);
	~ParticleShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

	void setBlend(float blend)
	{
		this->m_blend = blend;
	}

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer* m_transparentBuffer;
	float m_blend;
};

#endif