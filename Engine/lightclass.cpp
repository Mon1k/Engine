#include "lightclass.h"

LightClass::LightClass()
{
	m_specularPower = 0.0f;
	m_intensity = 1.5f;
	m_direction = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_type = LightType::LIGHT_POINT;
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
}


void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
}


D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

D3DXVECTOR3 LightClass::GetDirection()
{
	return m_direction;
}

D3DXVECTOR4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

float LightClass::GetSpecularPower()
{
	return m_specularPower;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR3 LightClass::GetPosition()
{
	return m_position;
}

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
}

void LightClass::GenerateViewMatrix()
{
	D3DXVECTOR3 up;

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_lookAt, &up);
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;

	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
}

void LightClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void LightClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void LightClass::GenerateOrthoMatrix(float width, float depthPlane, float nearPlane)
{
	// Create the orthographic matrix for the light.
	D3DXMatrixOrthoLH(&m_orthoMatrix, width, width, nearPlane, depthPlane);
}

void LightClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}
