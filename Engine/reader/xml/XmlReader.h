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
		file.open(&path[0], std::fstream::in);
		while (!file.eof()) {
			std::getline(file, data);
			parseLine(node, data);
		}
		file.close();

		return node;
	}

protected:
	void parseLine(Node* node, std::string line)
	{
		std::string data = line.substr(1, line.size() - 2);
	}
};