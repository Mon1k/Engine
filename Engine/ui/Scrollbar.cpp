#include "Scrollbar.h"
#include "group.h"

bool Scrollbar::onScroll(int diff)
{
	int sensibilityY = 5;
	float shift = (float)diff / sensibilityY;

	m_scrollY += -1 * shift;
	if (m_scrollY < 0) {
		m_scrollY = 0;
		return false;
	}

	float maxChildY = 0;
	for (size_t i = 0; i < m_childs.size(); i++) {
		maxChildY = max(maxChildY, m_childs[i]->m_y);
	}
	if (shift < 0 && m_parent->m_y + m_parent->m_height > maxChildY) {
		m_scrollY += shift;
		return false;
	}

	for (size_t i = 0; i < m_childs.size(); i++) {
		m_childs[i]->move(0, shift);
	}

	return true;
}

void Scrollbar::setParent(Group* parent)
{
	m_parent = parent;
	m_parent->addEventHandler(AbstractGui::EventType::SCROLL, [this] {
		m_childs = m_parent->getChilds();
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->m_y + m_childs[i]->m_height > m_parent->m_y + m_parent->m_height + 15 || m_childs[i]->m_y < m_parent->m_y) {
				m_childs[i]->hide();
			}
			else {
				m_childs[i]->show();
			}
		}
	});
}