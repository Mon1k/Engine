#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <d3dx10math.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);
	void SetPosition(float, float, float);
	void setPosition(D3DXVECTOR3 position) {
		m_position = position;
	}
	void setLookAt(D3DXVECTOR3 lookAt) {
		m_lookAt = lookAt;
	}
	void setIntensity(float intensity) {
		m_intensity = intensity;
	};

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 getLookAt() {
		return m_lookAt;
	};
	float getIntensity() {
		return m_intensity;
	};

	bool isDirection() {
		return m_direction.x != 0.0f || m_direction.y != 0.0f || m_direction.z != 0.0f;
	};

	void SetLookAt(float, float, float);
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);
	void GenerateOrthoMatrix(float, float, float);

	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

private:
	D3DXVECTOR4 m_ambientColor;
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specularColor;
	float m_specularPower;
	float m_intensity;

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;

	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_orthoMatrix;
};

#endif