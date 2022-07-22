#include "Blur.h"

bool Blur::Initialize(D3DClass* d3dClass, char* modelFilename)
{
	if (!ModelClass::Initialize(d3dClass, modelFilename, {})) {
		return false;
	}

	m_RenderToTexture = new RenderTextureClass;
	if (!m_RenderToTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_BlurShaderClass = new BlurShaderClass();
	if (!m_BlurShaderClass->Initialize(m_D3D->GetDevice())) {
		return false;
	}

	clearTargets();

	return true;
}

void Blur::PreRender(CameraClass* camera)
{
	if (m_modelsTarget.size() != 0) {
		RenderToTexture(camera);
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

void Blur::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ModelClass::Render();
	m_BlurShaderClass->Render(m_D3D->GetDeviceContext(), GetIndexCount(), this->GetWorldMatrix(), viewMatrix,
		projectionMatrix, m_RenderToTexture->GetShaderResourceView(), m_D3D->getScreenWidth() / 4, m_D3D->getScreenHeight() / 4);
	
}

void Blur::Shutdown()
{
	if (m_RenderToTexture) {
		m_RenderToTexture->Shutdown();
		delete m_RenderToTexture;
		m_RenderToTexture = 0;
	}
	if (m_BlurShaderClass) {
		m_BlurShaderClass->Shutdown();
		delete m_BlurShaderClass;
		m_BlurShaderClass = 0;
	}

	ModelClass::Shutdown();
}