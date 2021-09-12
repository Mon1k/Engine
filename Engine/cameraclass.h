#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


#include <d3dx10math.h>


class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetPosition(D3DXVECTOR3);
	void SetRotation(float, float, float);
	void SetRotation(D3DXVECTOR3);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);
	void setViewMatrix(D3DXMATRIX);
	D3DXMATRIX getBaseViewMatrix() {
		return m_baseViewMatrix;
	}
	void setBaseViewMatrix() {
		m_baseViewMatrix = m_viewMatrix;
	}

	void RenderReflection(float);
	D3DXMATRIX GetReflectionViewMatrix();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	
	D3DXMATRIX m_baseViewMatrix, m_viewMatrix;
	D3DXMATRIX m_reflectionViewMatrix;
};

#endif