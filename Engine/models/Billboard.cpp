#include "Billboard.h"

D3DXMATRIX Billboard::GetWorldMatrix()
{
	D3DXVECTOR3 cameraPosition = m_Camera->GetPosition();
	double angle;

	// Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
	angle = atan2(position.x - cameraPosition.x, position.z - cameraPosition.z) * (180.0 / D3DX_PI);

	// Convert rotation into radians.
	m_rotation.y = (float)angle * 0.0174532925f;

	return ModelClass::GetWorldMatrix();
}