#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

class RenderStencilTextureClass
{
public:
	RenderStencilTextureClass();
	~RenderStencilTextureClass();

	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenNear, float screenDepth, int depthArray = 1);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 0.0f);
	ID3D11ShaderResourceView* GetShaderResourceView();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	D3DXMATRIX GenerateOrthoMatrix(float minX, float minY, float maxX, float maxY, float nearPlane, float depthPlane);

private:
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11RasterizerState* m_shadowRasterizer;
	D3D11_VIEWPORT m_viewport;

	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;
};
