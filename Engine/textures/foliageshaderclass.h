#ifndef _FOLIAGESHADERCLASS_H_
#define _FOLIAGESHADERCLASS_H_


#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

#include "../shaders/AbstractShader.h"

class FoliageShaderClass : public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

public:
	FoliageShaderClass();
	FoliageShaderClass(const FoliageShaderClass&);
	~FoliageShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif