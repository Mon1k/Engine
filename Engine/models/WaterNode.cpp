#include "WaterNode.h"

bool WaterNode::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::wstring> texturesFilename)
{
	if (!Reflection::Initialize(d3dClass, modelFilename, texturesFilename)) {
		return false;
	}

	m_RefractionTexture = new RenderTextureClass;
	if (!m_RefractionTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight())) {
		return false;
	}

	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader->Initialize(m_D3D->GetDevice())) {
		MessageBox(NULL, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}

	m_WaterShader = new WaterShaderClass;
	if (!m_WaterShader->Initialize(m_D3D->GetDevice())) {
		MessageBox(NULL, L"Could not initialize the water shader object.", L"Error", MB_OK);
		return false;
	}

	m_LightShaderWater = new LightShaderClass;
	if (!m_LightShaderWater->Initialize(m_D3D->GetDevice())) {
		MessageBox(NULL, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void WaterNode::PreRender(CameraClass* camera)
{
	if (m_modelsTarget.size() != 0) {
		RenderRefractionToTexture(camera);
		RenderReflectionToTexture(camera);
	}
}

void WaterNode::RenderRefractionToTexture(CameraClass* camera)
{
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX viewMatrix, projectionMatrix;
	D3DXVECTOR3 position = this->GetPosition();

	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = D3DXVECTOR4(0.0f, position.y, 0.0f, m_waterHeight + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	LightShaderClass* shader = dynamic_cast<LightShaderClass*>(m_RefractionModel->getShader());
	LightClass* light = shader->getLight(0);

	m_RefractionModel->Render();
	m_RefractionShader->Render(m_D3D->GetDeviceContext(), m_RefractionModel->GetIndexCount(), m_RefractionModel->GetWorldMatrix(), viewMatrix,
		projectionMatrix, m_RefractionModel->GetTexture(), light->GetDirection(),
		light->GetAmbientColor(), light->GetDiffuseColor(), clipPlane);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}


void WaterNode::RenderReflectionToTexture(CameraClass* camera)
{
	D3DXMATRIX reflectionViewMatrix, projectionMatrix;

	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	camera->RenderReflection(m_waterHeight);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = camera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object.
	m_D3D->GetProjectionMatrix(projectionMatrix);

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		ModelClass* model = dynamic_cast<ModelClass*>(m_modelsTarget[i]);
		LightShaderClass* shader = dynamic_cast<LightShaderClass*>(model->getShader());
		m_LightShaderWater->addLights({ shader->getLight(0) });

		model->Render();
		m_LightShaderWater->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), reflectionViewMatrix, projectionMatrix,
			model->GetTextureArray(), camera->GetPosition());
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void WaterNode::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix, reflectionMatrix;

	reflectionMatrix = camera->GetReflectionViewMatrix();
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ModelClass::Render();

	m_WaterShader->Render(m_D3D->GetDeviceContext(), this->GetIndexCount(), this->GetWorldMatrix(), viewMatrix,
		projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(),
		m_RefractionTexture->GetShaderResourceView(), this->GetTexture(),
		m_waterTranslation, m_reflectRefractScale);
}

void WaterNode::Shutdown()
{
	if (m_LightShaderWater) {
		m_LightShaderWater->Shutdown();
		delete m_LightShaderWater;
		m_LightShaderWater = 0;
	}

	if (m_WaterShader) {
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}

	if (m_RefractionShader) {
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}

	if (m_RefractionTexture) {
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}

	Reflection::Shutdown();
}