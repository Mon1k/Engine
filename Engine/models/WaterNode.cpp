#include "WaterNode.h"
#include "Model.h"

WaterNode::WaterNode(): Reflection()
{

}

bool WaterNode::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::string> texturesFilename)
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

	m_RefractionModel = 0;

	m_waterHeight = position.y;

	// Set the tiling for the water normal maps.
	m_normalMapTiling.x = scale.x / 10.0f;  // Tile ten times over the quad.
	m_normalMapTiling.y = scale.z / 5.0f;  // Tile five times over the quad.
	
	// Initialize the water translation to zero.
	m_waterTranslation = 0;
	m_waterTranslationSpeed = 0.003f;
	m_counterFrame = 0;

	// Set the scaling value for the water normal map.
	m_reflectRefractScale = 0.03f;

	// Set the tint of the refraction.
	m_refractionTint = D3DXVECTOR4(0.0f, 0.8f, 1.0f, 1.0f);

	return true;
}

void WaterNode::PreRender(CameraClass* camera)
{
	RenderRefractionToTexture(camera);
	RenderReflectionToTexture(camera);
}

void WaterNode::RenderRefractionToTexture(CameraClass* camera)
{
	if (!m_RefractionModel) {
		return;
	}

	D3DXVECTOR4 clipPlane;
	D3DXMATRIX viewMatrix, projectionMatrix;
	D3DXVECTOR3 position = this->GetPosition();

	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = D3DXVECTOR4(0.0f, position.y, 0.0f, m_waterHeight + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	LightClass* light = m_RefractionModel->getLight(0);
	AbstractShader* shader = m_RefractionModel->getShader();

	if (shader) {
		m_RefractionModel->Render(camera);
	}
	else {
		m_RefractionModel->Render();
		m_RefractionShader->Render(m_D3D->GetDeviceContext(), m_RefractionModel->GetIndexCount(), m_RefractionModel->GetWorldMatrix(), viewMatrix,
			projectionMatrix, m_RefractionModel->GetTexture(), light->GetDirection(),
			light->GetAmbientColor(), light->GetDiffuseColor(), clipPlane);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}


void WaterNode::RenderReflectionToTexture(CameraClass* camera)
{
	if (m_modelsTarget.size() == 0) {
		return;
	}

	D3DXMATRIX reflectionViewMatrix, projectionMatrix, viewMatrix;
	D3DXVECTOR3 cameraPosition, oldCameraPosition;

	camera->GetViewMatrix(viewMatrix);

	cameraPosition = camera->GetPosition();
	oldCameraPosition = cameraPosition;

	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.8f, 1.0f, 0.5f);

	// Use the camera to render the reflection and create a reflection view matrix.
	camera->RenderReflection(m_waterHeight);

	// Get the camera reflection view matrix instead of the normal view matrix.
	m_ReflectionMatrix = camera->GetReflectionViewMatrix();

	camera->setViewMatrix(m_ReflectionMatrix);
	m_D3D->setViewMatrix(m_ReflectionMatrix);
	cameraPosition.y = -cameraPosition.y + (m_waterHeight * 2.0f);
	camera->SetPosition(cameraPosition);

	int size = m_modelsTarget.size();
	for (int i = 0; i < size; i++) {
		AbstractModel* model = dynamic_cast<AbstractModel*>(m_modelsTarget[i]);
		// skip refraction model if this Model (but not terrain sample)
		// @todo - why skip?
		if (model == m_RefractionModel && dynamic_cast<const Model*>(m_RefractionModel) != nullptr) {
			//continue;
		}
		model->Render(camera);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	camera->setViewMatrix(viewMatrix);
	m_D3D->setViewMatrix(viewMatrix);
	camera->SetPosition(oldCameraPosition);
}

void WaterNode::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	ModelClass::Render();

	m_WaterShader->Render(m_D3D->GetDeviceContext(), this->GetIndexCount(), this->GetWorldMatrix(), viewMatrix, projectionMatrix, m_ReflectionMatrix, 
		m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(), this->GetTexture(),
		camera->GetPosition(), m_normalMapTiling, m_waterTranslation, m_reflectRefractScale, m_refractionTint, getLight(0)->GetDirection(), getLight(0)->GetSpecularPower());
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

void WaterNode::frame(CameraClass* camera, float time)
{
	m_counterFrame += time;
	if (m_counterFrame > 50) {
		this->incrementTranslation();
		m_counterFrame = 0;
	}
}