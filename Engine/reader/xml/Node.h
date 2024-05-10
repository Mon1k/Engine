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
		child->parent = this;
		childs.push_back(child);
	}

	Attribute* getAttribute(std::string name)
	{
		for (int i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == name) {
				return attributes[i];
			}
		}

		return 0;
	}

	std::vector<Attribute*> getAttributes(std::string name)
	{
		std::vector<Attribute*> vector;
		vector.clear();

		for (int i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == name) {
				vector.push_back(attributes[i]);
			}
		}

		return vector;
	}

	std::vector<Attribute*> getAttributes()
	{
		return attributes;
	}

public:
	std::string name;
	std::vector<Attribute*> attributes;
	std::vector<Node*> childs;
	Node* parent;
};