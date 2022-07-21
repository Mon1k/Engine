#include "Blur.h"

bool Blur::Initialize(D3DClass* d3dClass, char* modelFilename)
{
	std::vector<std::wstring> texturesFilename = { L"data/textures/water01.dds" };
	if (!ModelClass::Initialize(d3dClass, modelFilename, texturesFilename)) {
		return false;
	}

	m_RenderToTexture = new RenderTextureClass;
	if (!m_RenderToTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_RenderToTextureBlur = new RenderTextureClass;
	if (!m_RenderToTextureBlur->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_RenderToTextureBig = new RenderTextureClass;
	if (!m_RenderToTextureBig->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_TextureShaderClass = new TextureShaderClass();
	if (!m_TextureShaderClass->Initialize(m_D3D->GetDevice())) {
		return false;
	}

	clearTargets();

	return true;
}

void Blur::PreRender(CameraClass* camera)
{
	if (m_modelsTarget.size() != 0) {
		m_WorldMatrix = GetWorldMatrix();
		RenderToTexture(camera);
		RenderToTextureBlur(camera);
		//RenderToTextureBig(camera);
	}
}

void Blur::RenderToTexture(CameraClass* camera)
{
	// Set the render target to be the render to texture.
	m_RenderToTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderToTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		m_modelsTarget[i]->Render(camera);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void Blur::RenderToTextureBlur(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, orthoMatrix, worldMatrix;

	
	// Set the render target to be the render to texture.
	m_RenderToTextureBlur->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderToTextureBlur->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	ModelClass::Render();
	HorizontalBlurShaderClass* shader = dynamic_cast<HorizontalBlurShaderClass*>(m_shader);
	shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), m_WorldMatrix, viewMatrix,
		orthoMatrix, m_RenderToTexture->GetShaderResourceView(), m_D3D->getScreenWidth());

	m_D3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void Blur::RenderToTextureBig(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, orthoMatrix;

	// Set the render target to be the render to texture.
	m_RenderToTextureBig->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderToTextureBig->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	ModelClass::Render();
	m_TextureShaderClass->Render(m_D3D->GetDeviceContext(), GetIndexCount(), m_WorldMatrix, viewMatrix,
		orthoMatrix, m_RenderToTextureBlur->GetShaderResourceView());

	m_D3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void Blur::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, orthoMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();

	ModelClass::Render();
	m_TextureShaderClass->Render(m_D3D->GetDeviceContext(), GetIndexCount(), m_WorldMatrix, viewMatrix,
		orthoMatrix, m_RenderToTextureBlur->GetShaderResourceView());

	m_D3D->TurnZBufferOn();
}

void Blur::Shutdown()
{
	if (m_RenderToTexture) {
		m_RenderToTexture->Shutdown();
		delete m_RenderToTexture;
		m_RenderToTexture = 0;
	}
	if (m_RenderToTextureBlur) {
		m_RenderToTextureBlur->Shutdown();
		delete m_RenderToTextureBlur;
		m_RenderToTextureBlur = 0;
	}
	if (m_RenderToTextureBig) {
		m_RenderToTextureBig->Shutdown();
		delete m_RenderToTextureBig;
		m_RenderToTextureBig = 0;
	}
	if (m_TextureShaderClass) {
		m_TextureShaderClass->Shutdown();
		delete m_TextureShaderClass;
		m_TextureShaderClass = 0;
	}

	ModelClass::Shutdown();
}