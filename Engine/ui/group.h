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

	virtual bool compareId(int id) {
		if (AbstractNode::compareId(id)) {
			return true;
		}

		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->compareId(id)) {
				return true;
			}
		}

		return false;
	}

	virtual AbstractNode* getById(int id) {
		if (AbstractNode::compareId(id)) {
			return this;
		}

		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible()) {
				AbstractGui* node = dynamic_cast<AbstractGui*>(m_childs[i]->getById(id));
				if (node) {
					return node;
				}
			}
		}

		return 0;
	}

	virtual void addChild(AbstractGui* child)
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

	virtual std::vector<AbstractGui*> getChilds()
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
		if (AbstractGui::isIntersect(x, y)) {
			return true;
		}

		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isIntersect(x, y)) {
				return true;
			}
		}

		return false;
	}

	virtual void proccesedEventHandlers(int event) {
		if (!isVisible()) {
			return;
		}

		int size = m_handlers.size();
		for (int i = 0; i < size; i++) {
			if (m_handlers[i].event == event) {
				m_handlers[i].handler();
			}

			int sizeChild = m_childs.size();
			for (int i = 0; i < sizeChild; i++) {
				if (m_childs[i]->isVisible() && m_childs[i]->m_handlers[i].event == event) {
					m_childs[i]->m_handlers[i].handler();
				}
			}
		}
	}

	virtual void onMousePress(int x, int y, int button)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isIntersect(x, y)) {
				m_childs[i]->onMousePress(x, y, button);
			}
		}
	}

	virtual void onKeyboardPress(int key, char symbol)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isFocused()) {
				m_childs[i]->onKeyboardPress(key, symbol);
			}
		}
	}

	virtual void frame(float counter)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible()) {
				m_childs[i]->frame(counter);
			}
		}
	}

	virtual bool isFocused()
	{
		if (m_visible && m_IsFocused) {
			return true;
		}

		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isFocused()) {
				return true;
			}
		}

		return false;
	}

protected:
	std::vector<AbstractGui*> m_childs;
};