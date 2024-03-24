#pragma once

#include <vector>

#include "AbstractGUI.h"
#include "image.h"
#include "label.h"

class ListNode : public AbstractGui
{
public:
	ListNode();

	virtual void initialize(std::string title, int width, int height, int positionX, int positionY);
	virtual void Shutdown();
	virtual bool Render();

	virtual void move(int dx, int dy);
	virtual int size();

	ListNode* addChild(std::string title);
	ListNode* addChild(ListNode* node)
	{
		m_childsNodes.push_back(node);
		return node;
	}
	void clearChilds();
	void calcPositionNodes(int positionY);

	std::string getTitle()
	{
		return m_title->getText();
	}

protected:
	Image* m_icon;
	Label* m_title;
	std::vector<ListNode*> m_childsNodes;
};
