#include "DebugWindow.h"

bool DebugWindow::Initialize(D3DClass* d3dClass, int width)
{
	m_D3D = d3dClass;

	m_x = 0;
	m_y = 0;

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader->Initialize(m_D3D->GetDevice())) {
		return false;
	}

	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	float divisor = m_D3D->getScreenWidth() / (float)m_D3D->getScreenHeight();
	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow->Initialize(m_D3D, m_D3D->getScreenWidth(), m_D3D->getScreenHeight(), width, (int)(width / divisor))) {
		MessageBox(NULL, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	m_modelsTarget.clear();

	return true;
}

void DebugWindow::PreRender(CameraClass* camera)
{
	if (m_modelsTarget.size() != 0) {
		RenderToTexture(camera);
	}
}

void DebugWindow::RenderToTexture(CameraClass* camera)
{
	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		m_modelsTarget[i]->Render(camera);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void DebugWindow::Render(CameraClass* camera)
{
	D3DXMATRIX worldMatrix, orthoMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	m_D3D->TurnZBufferOff();
	m_DebugWindow->Render(m_x, m_y);
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, camera->getBaseViewMatrix(),
		orthoMatrix, m_RenderTexture->GetShaderResourceView());
	m_D3D->TurnZBufferOn();

}

void DebugWindow::Shutdown()
{
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_DebugWindow) {
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}
}