#pragma once

#include <string>
#include <vector>

#include "../AbstractEntity.h"
#include "Attribute.h"

class Node: public AbstractEntity
{
public:
	Node()
	{
		name.clear();
		attributes.clear();
		childs.clear();
	}

	void addAttribute(Attribute* attribute)
	{
		attributes.push_back(attribute);
	}

	void removeAttribute(std::string name)
	{
		for (int i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == name) {
				attributes.erase(attributes.begin() + i);
			}
		}
	}

	void addChild(Node* child)
	{
		childs.push_back(child);
	}

public:
	std::string name;
	std::vector<Attribute*> attributes;
	std::vector<Node*> childs;
};