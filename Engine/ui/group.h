#pragma once

#include <vector>
#include <algorithm>

#include "AbstractGUI.h"
#include "Scrollbar.h"

class Group : public AbstractGui
{
public:
	Group()
	{
		m_childs.clear();
		m_scrollbar = 0;
	}

	virtual bool compareId(int id) {
		if (AbstractNode::compareId(id)) {
			return true;
		}

		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->compareId(id)) {
				return true;
			}
		}

		return false;
	}

	virtual int getLastId()
	{
		int id = 0;
		for (size_t i = 0; i < m_childs.size(); i++) {
			int elmId = 0;
			if (dynamic_cast<Group*>(m_childs[i]) != nullptr) {
				elmId = ((Group*)m_childs[i])->getLastId();
			}
			if (m_childs[i]->getId() > elmId) {
				elmId = m_childs[i]->getId();
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

		for (size_t i = 0; i < m_childs.size(); i++) {
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
		child->m_parent = this;
		m_childs.push_back(child);

		return child;
	}

	virtual void removeChild(int id)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->getId() == id) {
				m_childs[i]->Shutdown();
				m_childs.erase(m_childs.begin() + i);
			}
		}
	}

	virtual void removeChild(AbstractGui* ui)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i] == ui) {
				m_childs[i]->Shutdown();
				m_childs.erase(m_childs.begin() + i);
			}
		}
	}

	virtual void Shutdown()
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			m_childs[i]->Shutdown();
		}
		m_childs.clear();
	}

	virtual bool Render()
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible()) {
				m_childs[i]->Render();
			}
		}

		return true;
	}

	virtual std::vector<AbstractGui*> getChilds()
	{
		return m_childs;
	}

	virtual void setPosition(float x, float y)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			m_childs[i]->setPosition(x, y);
		}
	}

	virtual bool isIntersect(int x, int y)
	{
		if (AbstractGui::isIntersect(x, y)) {
			return true;
		}

		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isIntersect(x, y)) {
				return true;
			}
		}

		return false;
	}

	virtual bool proccesedEventHandlers(int event)
	{
		if (!isVisible()) {
			return false;
		}

		bool isEvent = false;
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible()) {
				if (m_childs[i]->proccesedEventHandlers(event)) {
					isEvent = true;
					break;
				}
			}
		}

		if (!isEvent) {
			for (size_t i = 0; i < m_handlers.size(); i++) {
				if (m_handlers[i].event == event) {
					m_handlers[i].handler();
					isEvent = true;
				}
			}
		}

		return isEvent;
	}

	virtual bool onMousePress(int x, int y, int button)
	{
		bool result = false;
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (!result && m_childs[i]->isVisible() && m_childs[i]->isIntersect(x, y)) {
				m_childs[i]->focus();
				result = m_childs[i]->onMousePress(x, y, button);
			}
			else {
				m_childs[i]->unfocus();
			}
		}

		return result;
	}

	virtual bool onKeyboardPress(InputClass::EventKey event)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isFocused()) {
				if (m_childs[i]->onKeyboardPress(event)) {
					return true;
				}
			}
			else {
				m_childs[i]->unfocus();
			}
		}

		return false;
	}

	virtual bool onScroll(int diff)
	{
		AbstractGui::onScroll(diff);

		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible()) {
				m_childs[i]->onScroll(diff);
			}
		}

		if (m_scrollbar) {
			m_scrollbar->onScroll(diff);
		}

		return true;
	}

	virtual void frame(float counter)
	{
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible()) {
				m_childs[i]->frame(counter);
				if (!m_IsFocused && m_childs[i]->isFocused()) {
					m_IsFocused = true;
				}
			}
		}
	}

	virtual bool isFocused()
	{
		if (m_visible && m_IsFocused) {
			return true;
		}

		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible() && m_childs[i]->isFocused()) {
				return true;
			}
		}

		return false;
	}

	virtual void focus()
	{
		if (!m_IsFocused) {
			m_IsFocused = true;
		}
	}

	virtual void unfocus()
	{
		if (m_IsFocused) {
			m_IsFocused = false;

			for (size_t i = 0; i < m_childs.size(); i++) {
				if (m_childs[i]->isVisible() && m_childs[i]->isFocused()) {
					m_childs[i]->unfocus();
				}
			}
		}
	}

	virtual void move(int dx, int dy)
	{
		AbstractGui::move(dx, dy);

		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->isVisible()) {
				m_childs[i]->move(dx, dy);
			}
		}
	}

	void addScrollbar(Group* owner)
	{
		m_scrollbar = new Scrollbar;
		m_scrollbar->setParent(owner);
	}

protected:
	std::vector<AbstractGui*> m_childs;
	Scrollbar* m_scrollbar;
};