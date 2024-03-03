#include "Reflection.h"

Reflection::Reflection() : ModelClass()
{

}

bool Reflection::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::string> texturesFilename)
{
	if (!ModelClass::Initialize(d3dClass, modelFilename, texturesFilename)) {
		return false;
	}

	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	clearTargets();

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
	D3DXMATRIX reflectionViewMatrix, projectionMatrix, viewMatrix;

	camera->GetViewMatrix(viewMatrix);

	// Set the render target to be the render to texture.
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	camera->RenderReflection(position.y);

	// Get the camera reflection view matrix instead of the normal view matrix.
	m_ReflectionMatrix = camera->GetReflectionViewMatrix();

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		camera->setViewMatrix(m_ReflectionMatrix);
		m_D3D->setViewMatrix(m_ReflectionMatrix);
		m_modelsTarget[i]->Render(camera);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
	camera->setViewMatrix(viewMatrix);
	m_D3D->setViewMatrix(viewMatrix);
}

void Reflection::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	ModelClass::Render();

	if (m_shader) {
		camera->GetViewMatrix(viewMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);

		ReflectionShaderClass* shader = dynamic_cast<ReflectionShaderClass*>(m_shader);
		shader->Render(m_D3D->GetDeviceContext(), GetIndexCount(), GetWorldMatrix(), viewMatrix,
			projectionMatrix, GetTexture(), m_ReflectionTexture->GetShaderResourceView(), m_ReflectionMatrix);
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