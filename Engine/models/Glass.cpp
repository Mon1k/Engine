#include "Glass.h"


bool Glass::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::string> texturesFilename)
{
	if (!ModelClass::Initialize(d3dClass, modelFilename, texturesFilename)) {
		return false;
	}

	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_GlassShader = new GlassShaderClass;
	if (!m_GlassShader->Initialize(m_D3D->GetDevice())) {
		MessageBox(NULL, L"Could not initialize the glass shader object.", L"Error", MB_OK);
		return false;
	}

	clearTargets();

	m_refractionScale = 0.01f;

	return true;
}

void Glass::PreRender(CameraClass* camera)
{
	if (m_modelsTarget.size() != 0) {
		RenderToTexture(camera);
	}
}

void Glass::RenderToTexture(CameraClass* camera)
{
	// Set the render target to be the render to texture.
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	camera->Render();

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		m_modelsTarget[i]->Render(camera);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void Glass::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ModelClass::Render();

	m_GlassShader->Render(m_D3D->GetDeviceContext(), this->GetIndexCount(), this->GetWorldMatrix(), viewMatrix, projectionMatrix,
		this->GetTexture(), this->GetTexture(1), m_ReflectionTexture->GetShaderResourceView(),
		m_refractionScale);
}

void Glass::Shutdown()
{
	if (m_ReflectionTexture) {
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	if (m_GlassShader) {
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = 0;
	}

	ModelClass::Shutdown();
}