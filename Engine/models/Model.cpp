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

void Model::Render()
{
	if (m_BBox) {
		m_BBox->Render();
	}
	
	ModelClass::Render();
}

BBox* Model::showBBox()
{
	if (!m_BBox) {
		D3DXVECTOR3 position, size;
		GetBoundingBox(position, size);

		m_BBox = new BBox;
		m_BBox->CreateBox(m_D3D, position, size);
	}

	return m_BBox;
}

void Model::hideBBox()
{
	if (m_BBox) {
		m_BBox->Shutdown();
		delete m_BBox;
		m_BBox = 0;
	}
}