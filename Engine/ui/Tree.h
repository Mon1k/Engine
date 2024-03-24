#pragma once

#include <vector>

#include "window.h"
#include "ListNode.h"

class Tree : public Window
{
public:
	Tree();

	virtual bool initialize(int width, int height, int positionX, int positionY);
	virtual void Shutdown();

	ListNode* addNode(std::string title);
	std::vector<ListNode*> getChildsNodes()
	{
		return m_childsNodes;
	}


protected:
	ListNode* m_parentNode;
	std::vector<ListNode*> m_childsNodes;
};