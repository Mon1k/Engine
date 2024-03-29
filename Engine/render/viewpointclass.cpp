#include "viewpointclass.h"


ViewPointClass::ViewPointClass()
{
}


ViewPointClass::ViewPointClass(const ViewPointClass& other)
{
}


ViewPointClass::~ViewPointClass()
{
}


void ViewPointClass::SetPosition(float x, float y, float z)
{
	SetPosition(D3DXVECTOR3(x, y, z));
}

void ViewPointClass::SetPosition(D3DXVECTOR3 position)
{
	m_position = position;
}

void ViewPointClass::SetLookAt(float x, float y, float z)
{
	SetLookAt(D3DXVECTOR3(x, y, z));
}

void ViewPointClass::SetLookAt(D3DXVECTOR3 lookAt)
{
	m_lookAt = lookAt;
}

void ViewPointClass::SetProjectionParameters(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
}


void ViewPointClass::GenerateViewMatrix()
{
	D3DXVECTOR3 up;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&m_viewMatrix, &m_position, &m_lookAt, &up);
}


void ViewPointClass::GenerateProjectionMatrix()
{
	// Create the projection matrix for the view point.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}


void ViewPointClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}


void ViewPointClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}