#include "Fire.h"

bool Fire::Initialize(D3DClass* d3dClass, char* modelFilename, std::vector<std::string> texturesFilename)
{
	if (!ModelClass::Initialize(d3dClass, modelFilename, texturesFilename)) {
		return false;
	}

	m_FireShader = new FireShaderClass;

	if (!m_FireShader->Initialize(m_D3D->GetDevice()))
	{
		MessageBox(NULL, L"Could not initialize the fire shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the three scrolling speeds for the three different noise textures.
	m_scrollSpeeds = D3DXVECTOR3(1.3f, 2.1f, 2.3f);

	// Set the three scales which will be used to create the three different noise octave textures.
	m_scales = D3DXVECTOR3(1.0f, 2.0f, 3.0f);

	// Set the three different x and y distortion factors for the three different noise textures.
	m_distortion1 = D3DXVECTOR2(0.1f, 0.2f);
	m_distortion2 = D3DXVECTOR2(0.1f, 0.3f);
	m_distortion3 = D3DXVECTOR2(0.1f, 0.1f);

	m_distortionScale = 0.8f;
	m_distortionBias = 0.5f;

	m_frameTime = 0.0f;

	return true;
}

void Fire::Render(CameraClass* camera)
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_D3D->TurnOnAlphaBlending();

	ModelClass::Render();

	m_FireShader->Render(m_D3D->GetDeviceContext(), this->GetIndexCount(), this->GetWorldMatrix(), viewMatrix, projectionMatrix,
		this->GetTexture(), this->GetTexture(1), this->GetTexture(2), m_frameTime, m_scrollSpeeds,
		m_scales, m_distortion1, m_distortion2, m_distortion3, m_distortionScale, m_distortionBias);

	m_D3D->TurnOffAlphaBlending();
}

void Fire::Shutdown()
{
	if (m_FireShader) {
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
	}

	ModelClass::Shutdown();
}