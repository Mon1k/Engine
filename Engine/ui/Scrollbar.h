#pragma once

#include <vector>

#include "AbstractGUI.h"

class Group;

class Scrollbar
{
public:
	Scrollbar()
	{
	}

	virtual bool onScroll(int diff);
	void setParent(Group* parent);

protected:
	int m_scrollY = 0;
	Group* m_parent;

	std::vector<AbstractGui*> m_childs;
};