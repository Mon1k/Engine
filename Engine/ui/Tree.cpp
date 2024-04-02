#include "Tree.h"
#include "UIManager.h"

Tree::Tree(): Window()
{

}

bool Tree::initialize(int width, int height, int positionX, int positionY)
{
	Window::Initialize(width, height, positionX, positionY);

	setId(m_manager->getNextId());

	addHeader("Tree", HEADER_BUTTON_CLOSE);
	addBody();

	m_parentNode = new ListNode;

	addScrollbar(m_body);

	return true;
}

void Tree::Shutdown()
{
	m_parentNode->Shutdown();

	Window::Shutdown();
}

ListNode* Tree::addNode(std::string title)
{
	int padding = 5;
	int top = m_y + m_header->m_height + padding;

	ListNode* node = new ListNode();
	addChild(node);
	node->m_parent = m_parentNode;

	size_t size = m_childsNodes.size();
	for (size_t i = 0; i < m_childsNodes.size(); i++) {
		size += m_childsNodes[i]->size();
	}

	node->initialize(title, m_width - padding * 2, node->m_height, m_x + padding, top + size * (node->m_height + padding));
	m_childsNodes.push_back(node);
	m_parentNode->addChild(node);

	//proccesedEventHandlers(AbstractGui::EventType::SCROLL);

	return node;
}