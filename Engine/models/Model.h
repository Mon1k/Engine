#pragma once

#include "../modelclass.h"
#include "bbox.h"

class Model : public ModelClass
{
public:
	Model();

	virtual void Shutdown();
	virtual void Render();

	BBox* showBBox();
	void hideBBox();
	BBox* getBBox()
	{
		return m_BBox;
	}

protected:
	BBox* m_BBox;
};