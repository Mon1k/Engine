#include "Model.h"

Model::Model() : ModelClass()
{
	m_BBox = 0;
}

void Model::Shutdown()
{
	if (m_BBox) {
		m_BBox->Shutdown();
		delete m_BBox;
		m_BBox = 0;
	}

	ModelClass::Shutdown();
}

void Model::Render(CameraClass* camera)
{
	ModelClass::Render(camera);
	
	if (m_BBox) {
		m_BBox->Render(camera);
	}
}

void Model::showBBox()
{
	if (!m_BBox) {
		D3DXVECTOR3 position, size;
		GetBoundingBox(position, size);

		m_BBox = new BBox;
		m_BBox->CreateBox(m_D3D, position, size);
	}
}

void Model::hideBBox()
{
	if (m_BBox) {
		m_BBox->Shutdown();
		delete m_BBox;
		m_BBox = 0;
	}
}