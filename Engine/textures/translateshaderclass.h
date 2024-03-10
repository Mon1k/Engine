#ifndef _TRANSLATESHADERCLASS_H_
#define _TRANSLATESHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include "../shaders/AbstractShader.h"

class TranslateShaderClass: public AbstractShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct TranslateBufferType
	{
		float translationX;
		float translationY;
		int frameX;
		int frameY;
		int maxXFrame;
		int maxYFrame;
		int padding1;
		int padding2;
	};

public:
	TranslateShaderClass();
	TranslateShaderClass(const TranslateShaderClass&);
	~TranslateShaderClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3);
	void incrementFrame();
	void setFrame(int x, int y) {
		currentXFrame = x;
		currentYFrame = y;
	};
	void setMaxFrame(int x, int y) {
		maxXFrame = x;
		maxYFrame = y;
	};

private:
	bool InitializeShader(ID3D11Device*, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer* m_translateBuffer;

	float currentXPosition, currentYPosition;
	int currentXFrame, currentYFrame;
	int maxXFrame, maxYFrame;
};

#endif