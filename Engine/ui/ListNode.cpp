#include "ListNode.h"
#include "Tree.h"

ListNode::ListNode()
{
	m_icon = 0;
	m_title = 0;

	m_height = 28;
}

void ListNode::initialize(std::string title, int width, int height, int positionX, int positionY)
{
	m_width = width;
	m_height = height;
	m_x = positionX;
	m_y = positionY;

	m_title = new Label;
	m_title->m_baseViewMatrix = m_baseViewMatrix;
	m_title->m_D3D = m_D3D;
	m_title->m_parent = this;
	m_title->m_manager = m_manager;
	m_title->Initialize(width, height);
	m_title->Add(&title[0], positionX, positionY);
}

void ListNode::Shutdown()
{
	if (m_icon) {
		m_icon->Shutdown();
		m_icon = 0;
	}

	if (m_title) {
		m_title->Shutdown();
		m_title = 0;
	}

	clearChilds();
}

bool ListNode::Render()
{
	if (m_icon) {
		m_icon->Render();
	}

	m_title->Render();

	for (size_t i = 0; i < m_childsNodes.size(); i++) {
		m_childsNodes[i]->Render();
	}

	return true;
}

void ListNode::move(int dx, int dy)
{
	AbstractGui::move(dx, dy);

	if (m_icon) {
		m_icon->move(dx, dy);
	}

	m_title->move(dx, dy);

	for (size_t i = 0; i < m_childsNodes.size(); i++) {
		m_childsNodes[i]->move(dx, dy);
	}
}

ListNode* ListNode::addChild(std::string title)
{
	int padding = 5;
	int shiflLevel = 15;
	int positionY = m_y + m_height + padding + size() * m_height;

	ListNode* node = new ListNode();
	node->m_baseViewMatrix = m_baseViewMatrix;
	node->m_D3D = m_D3D;
	node->m_parent = this;
	node->m_manager = m_manager;
	node->initialize(title, m_width - padding * 2, m_height, m_x + padding + shiflLevel, positionY);
	
	m_childsNodes.push_back(node);
	if (m_parent && dynamic_cast<ListNode*>(m_parent) != nullptr) {
		ListNode* parent = dynamic_cast<ListNode*>(m_parent);
		parent->calcPositionNodes(positionY);
	}

	return node;
}

void ListNode::calcPositionNodes(int positionY)
{
	int newPositionY = positionY;
	int padding = 5;

	for (size_t i = 0; i < m_childsNodes.size(); i++) {
		if (m_childsNodes[i]->m_y <= positionY && m_childsNodes[i]->m_y + m_childsNodes[i]->m_height >= positionY) {
			m_childsNodes[i]->move(0, positionY - m_childsNodes[i]->m_y + m_childsNodes[i]->m_height);
			newPositionY = max(newPositionY, m_childsNodes[i]->m_y);
		}
		// if empty childs then update more value for shift
		else if (!m_parent && m_childsNodes[i]->size() == 0 && m_childsNodes[i]->m_y <= positionY) {
			m_childsNodes[i]->move(0, positionY - m_childsNodes[i]->m_y + m_childsNodes[i]->m_height);
			newPositionY = max(newPositionY, m_childsNodes[i]->m_y);
		}
	}

	if (m_parent && dynamic_cast<ListNode*>(m_parent) != nullptr) {
		ListNode* parent = dynamic_cast<ListNode*>(m_parent);
		parent->calcPositionNodes(newPositionY);
	}
}

void ListNode::clearChilds()
{
	for (size_t i = 0; i < m_childsNodes.size(); i++) {
		m_childsNodes[i]->Shutdown();
	}
	m_childsNodes.clear();
}

int ListNode::size()
{
	size_t size = m_childsNodes.size();
	for (size_t i = 0; i < m_childsNodes.size(); i++) {
		size += m_childsNodes[i]->size();
	}

	return size;
}