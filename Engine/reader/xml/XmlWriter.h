#pragma once

#include <fstream>

#include "../AbstractWriter.h"
#include "Node.h"

class XmlWriter : public AbstractWriter
{
public:
	virtual void save(std::string path, AbstractEntity* entity)
	{
		Node* node = static_cast<Node*>(entity);
		std::string data = getNodeString(node);

		std::fstream file;
		file.open(&path[0], std::fstream::out);
		file.write(&data[0], data.size());
		file.close();
	}

	std::string getNodeString(Node* node)
	{
		std::string data;
		data.clear();

		data = "<" + node->name;

		if (node->attributes.size() > 0) {
			for (int i = 0; i < node->attributes.size(); i++) {
				data += " " + node->attributes[i]->name + "=\"" + node->attributes[i]->value + "\"";
			}
		}
		data += ">";

		if (node->childs.size() > 0) {
			for (int i = 0; i < node->childs.size(); i++) {
				data += "\n\t" + getNodeString(node->childs[i]);
			}
		}
		data += "\r</" + node->name + ">";

		return data;
	}
};
