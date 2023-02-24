#ifndef _VIEWPOINTCLASS_H_
#define _VIEWPOINTCLASS_H_

#include <d3dx10math.h>

class ViewPointClass
{
public:
	ViewPointClass();
	ViewPointClass(const ViewPointClass&);
	~ViewPointClass();

	void SetPosition(float, float, float);
	void SetPosition(D3DXVECTOR3);
	void SetLookAt(float, float, float);
	void SetLookAt(D3DXVECTOR3);
	void SetProjectionParameters(float, float, float, float);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix();

	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);

	D3DXVECTOR3 getPosition() {
		return m_position;
	}
	D3DXVECTOR3 getLookAt() {
		return m_lookAt;
	}

private:
	D3DXVECTOR3 m_position, m_lookAt;
	D3DXMATRIX m_viewMatrix, m_projectionMatrix;
	float m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane;
};

#endif