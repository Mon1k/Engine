#pragma once

#include "../reader/AbstractReader.h"
#include "../reader/xml/Node.h"
#include "MapEntity.h"
#include "../models/ModelManager.h"

class XmlLoad
{
public:
	XmlLoad(AbstractReader* reader)
	{
		this->reader = reader;
	}

	void load(std::string path, std::vector<MapEntity::ObjectFormat> entities, ModelManager* manager)
	{
		Node* node = static_cast<Node*>(this->reader->read(path));
	}

	D3DXVECTOR3 extractVector3(Attribute* attribute)
	{
		D3DXVECTOR3 vector;

		return vector;
	}

public:
	AbstractReader* reader;
};