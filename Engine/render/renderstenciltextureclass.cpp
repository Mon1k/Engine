#include "RenderStencilTextureClass.h"

RenderStencilTextureClass::RenderStencilTextureClass()
{
	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;
	m_shaderResourceView = 0;
	m_shadowRasterizer = 0;
}

RenderStencilTextureClass::~RenderStencilTextureClass()
{
}

bool RenderStencilTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenNear, float screenDepth, int depthArray)
{
	// setup the description of the depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.MiscFlags = 0;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateTexture2D(&depthBufferDesc, 0, &m_depthStencilBuffer);

	// setup the depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);

	// setup shader resource
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	if (depthArray == 6) {
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDesc.TextureCube.MipLevels = 1;
		shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
	} else {
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	}
	device->CreateShaderResourceView(m_depthStencilBuffer, &shaderResourceViewDesc, &m_shaderResourceView);

	// setup rasterizer
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible positive value storable in the depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &m_shadowRasterizer);

	// setup the viewport for rendering
	m_viewport = {};
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// setup the projection matrix
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, ((float)D3DX_PI / 3.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	// create an orthographic projection matrix for 2D rendering
	D3DXMatrixOrthoLH(&m_orthoMatrix, textureWidth / 7, textureWidth / 7, screenNear, screenDepth);

	return true;
}

void RenderStencilTextureClass::Shutdown()
{
	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_shaderResourceView) {
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_shadowRasterizer) {
		m_shadowRasterizer->Release();
		m_shadowRasterizer = 0;
	}
}

void RenderStencilTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(0, 0, m_depthStencilView);
}

void RenderStencilTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext,
	float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->RSSetState(m_shadowRasterizer);
}

D3DXMATRIX RenderStencilTextureClass::GenerateOrthoMatrix(float minX, float minY, float maxX, float maxY, float nearPlane, float depthPlane)
{
	D3DXMatrixOrthoOffCenterLH(&m_orthoMatrix, minX, maxX, minY, maxY, nearPlane, depthPlane);

	return m_orthoMatrix;
}

ID3D11ShaderResourceView* RenderStencilTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}

void RenderStencilTextureClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void RenderStencilTextureClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}