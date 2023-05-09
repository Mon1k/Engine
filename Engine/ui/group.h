#pragma once

#include <vector>
#include <algorithm>
using namespace std;

#include "AbstractGUI.h"

class Group : public AbstractGui
{
public:
	Group() {
		m_childs.clear();
	}

	void addChild(AbstractGui* child)
	{
		child->m_D3D = m_D3D;
		child->m_baseViewMatrix = m_baseViewMatrix;
		m_childs.push_back(child);
	}

	virtual bool Render()
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible()) {
				m_childs[i]->Render();
			}
		}

		return true;
	}

	virtual void Shutdown()
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->Shutdown();
		}
	}

	std::vector<AbstractGui*> getChilds()
	{
		return m_childs;
	}

	virtual void setPosition(float x, float y)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->setPosition(x, y);
		}
	}

	virtual bool isIntersect(int x, int y)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isIntersect(x, y)) {
				return true;
			}
		}

		return false;
	}

	virtual void onMousePress(int x, int y, int button)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->onMousePress(x, y, button);
		}
	}

	virtual void onKeyboardPress(int key, char symbol)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->onKeyboardPress(key, symbol);
		}
	}

	virtual void frame(float counter)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			m_childs[i]->frame(counter);
		}
	}

	virtual bool isFocused()
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isFocused()) {
				return true;
			}
		}

		return false;
	}

private:
	std::vector<AbstractGui*> m_childs;
};