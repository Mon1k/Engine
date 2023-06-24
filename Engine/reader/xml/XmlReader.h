#pragma once

#include <fstream>
#include <string>

#include "../AbstractReader.h"
#include "Node.h"

class XmlReader: public AbstractReader
{
public:
	Node* read(std::string path)
	{
		Node* node = new Node;
		std::string data = "";
		std::ifstream file;

		Node* tag = node;
		file.open(&path[0], std::fstream::in);
		while (!file.eof()) {
			std::getline(file, data);
			tag = parseLine(tag, data);
		}
		file.close();

		return node;
	}

protected:
	Node* parseLine(Node* node, std::string line)
	{
		line.erase(0, line.find_first_not_of(" \n\r\t"));

		if (line.substr(0, 2) == "</") {
			return node->parent;
		}

		int space = line.find_first_of(" ");
		if (space == -1) {
			space = line.size() - 1;
		} else if (space == line.size()) {
			space--;
		}
		std::string tagName = line.substr(1, space - 1);
		
		Node* child = new Node;
		child->name = tagName;

		int nextData = 1 + tagName.size() + 1;
		if (nextData < line.size() - 1) {
			parseAttribute(child, line.substr(nextData, line.size() - 1));
		}

		node->addChild(child);

		return child;
	}

	void parseAttribute(Node* node, std::string stringAttrubutes)
	{
		std::vector<std::string> attributesScopes;
		attributesScopes.clear();

		int shift = 0;
		do {
			std::string attributeScope = "";
			int nextSpace = stringAttrubutes.find_first_of(" ", shift);
			if (nextSpace == -1) {
				nextSpace = stringAttrubutes.size() - 1;
			}

			attributeScope = stringAttrubutes.substr(shift, nextSpace - shift - 1);
			attributesScopes.push_back(attributeScope);
			shift = nextSpace + 1;
		} while (shift < stringAttrubutes.size());

		extractAttribte(node, attributesScopes);
	}

	void extractAttribte(Node* node, std::vector<std::string> attributesScopes)
	{
		int size = attributesScopes.size();

		for (int i = 0; i < size; i++) {
			Attribute* attribute = new Attribute;
			int pos = attributesScopes[i].find_first_of("=");

			attribute->name = attributesScopes[i].substr(0, pos);
			attribute->value = attributesScopes[i].substr(attribute->name.size() + 2, attributesScopes[i].size() - 1);
			node->addAttribute(attribute);
		}
	}
};