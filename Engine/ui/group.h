#pragma once

#include <vector>
#include <algorithm>

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

	virtual int getLastId()
	{
		int id = 0;
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			int elmId = m_childs[i]->getId();
			if (dynamic_cast<Group*>(m_childs[i]) != nullptr) {
				elmId = ((Group*)m_childs[i])->getLastId();
			}
			if (elmId > id) {
				id = elmId;
			}
		}

		return id;
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

	virtual AbstractGui* addChild(AbstractGui* child)
	{
		child->m_D3D = m_D3D;
		child->m_baseViewMatrix = m_baseViewMatrix;
		child->m_manager = m_manager;
		m_childs.push_back(child);

		return child;
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

		size_t size = m_handlers.size();
		for (size_t i = 0; i < size; i++) {
			if (m_handlers[i].event == event) {
				m_handlers[i].handler();
			}
		}

		size_t sizeChild = m_childs.size();
		for (size_t i = 0; i < sizeChild; i++) {
			if (m_childs[i]->isVisible()) {
				size_t sizeChildHandler = m_childs[i]->m_handlers.size();
				for (size_t j = 0; j < sizeChildHandler; j++) {
					if (m_childs[i]->m_handlers[j].event == event) {
						m_childs[i]->m_handlers[j].handler();
					}
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

	virtual void onKeyboardPress(InputClass::EventKey event)
	{
		int size = m_childs.size();
		for (int i = 0; i < size; i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isFocused()) {
				m_childs[i]->onKeyboardPress(event);
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