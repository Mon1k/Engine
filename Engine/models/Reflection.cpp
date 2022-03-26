#include "Reflection.h"


bool Reflection::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::wstring> texturesFilename)
{
	if (!ModelClass::Initialize(d3dClass, modelFilename, texturesFilename)) {
		return false;
	}

	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_modelsTarget.clear();

	return true;
}

void Reflection::PreRender(CameraClass* camera)
{
	if (m_modelsTarget.size() != 0) {
		RenderToTexture(camera);
	}
}

void Reflection::RenderToTexture(CameraClass* camera)
{
	D3DXMATRIX reflectionViewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	camera->RenderReflection(-5.0f);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = camera->GetReflectionViewMatrix();
	m_D3D->GetProjectionMatrix(projectionMatrix);

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		m_modelsTarget[i]->Render(camera);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void Reflection::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix, reflectionMatrix;

	ModelClass::Render();

	if (m_shader) {
		reflectionMatrix = camera->GetReflectionViewMatrix();
		camera->GetViewMatrix(viewMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);

		ReflectionShaderClass* shader = dynamic_cast<ReflectionShaderClass*>(m_shader);
		shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), GetWorldMatrix(), viewMatrix,
			projectionMatrix, GetTextureArray()[0], m_ReflectionTexture->GetShaderResourceView(), reflectionMatrix);
	}
}

void Reflection::Shutdown()
{
	if (m_ReflectionTexture) {
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	ModelClass::Shutdown();
}