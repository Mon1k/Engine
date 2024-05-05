#pragma once

#include "../modelclass.h"
#include "bbox.h"

class Model : public ModelClass
{
public:
	Model();

	virtual void Shutdown();
	virtual void Render();
	virtual void Render(CameraClass* camera);

	BBox* showBBox();
	void hideBBox();
	void refreshBBox()
	{
		hideBBox();
		showBBox();
	}
	BBox* getBBox()
	{
		return m_BBox;
	}

protected:
	BBox* m_BBox;
};