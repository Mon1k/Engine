#pragma once

#include "../modelclass.h"
#include "../cameraclass.h"

class Billboard : public ModelClass
{
public:
	D3DXMATRIX GetWorldMatrix();
	void setCamera(CameraClass* camera) {
		m_Camera = camera;
	}

private:
	CameraClass* m_Camera;
};