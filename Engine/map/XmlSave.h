#pragma once

#include <string>

#include "../reader/AbstractWriter.h"
#include "../reader/xml/Node.h"
#include "MapEntity.h"

class XmlSave
{
public:
	XmlSave(AbstractWriter* writer)
	{
		this->writer = writer;
	}

	void save(std::string path, std::vector<MapEntity::ObjectFormat> entities)
	{
		this->writer->save(path, getNode(entities));
	}

	Node* getNode(std::vector<MapEntity::ObjectFormat> entities)
	{
		Node* node = new Node();
		node->name = "world";

		getModelsNode(node, entities);

		return node;
	}

	void getModelsNode(Node* node, std::vector<MapEntity::ObjectFormat> entities)
	{
		for (int i = 0; i < entities.size(); i++) {
			if (entities[i].type == MapEntity::ObjectTypes::MODEL) {
				Node* child = new Node;

				child->name = "model";
				fillAttributes(child, entities[i]);

				node->addChild(child);
			}
		}
	}

	void fillAttributes(Node* node, MapEntity::ObjectFormat entity)
	{
		if (entity.position) {
			Attribute* position = new Attribute;
			position->name = "position";
			position->value = std::to_string(entity.position.x) + ":" + std::to_string(entity.position.y) + ":" + std::to_string(entity.position.z);
			node->addAttribute(position);
		}

		if (entity.scale) {
			Attribute* scale = new Attribute;
			scale->name = "scale";
			scale->value = std::to_string(entity.scale.x) + ":" + std::to_string(entity.scale.y) + ":" + std::to_string(entity.scale.z);
			node->addAttribute(scale);
		}

		if (entity.rotation) {
			Attribute* rotation = new Attribute;
			rotation->name = "rotation";
			rotation->value = std::to_string(entity.rotation.x) + ":" + std::to_string(entity.rotation.y) + ":" + std::to_string(entity.rotation.z);
			node->addAttribute(rotation);
		}

		if (entity.path.size() > 0) {
			Attribute* path = new Attribute;
			path->name = "path";
			path->value = entity.path;
			node->addAttribute(path);
		}

		if (entity.texture.size() > 0) {
			Attribute* texture = new Attribute;
			texture->name = "texture";
			texture->value = entity.texture;
			node->addAttribute(texture);
		}

		if (entity.type != 0) {
			Attribute* type = new Attribute;
			type->name = "type";
			type->value = std::to_string(entity.type);
			node->addAttribute(type);
		}

		if (entity.extraParams.size() > 0) {
			for (int i = 0; i < entity.extraParams.size(); i++) {
				if (entity.extraParams[i].size() > 0) {
					Attribute* extraParams = new Attribute;
					extraParams->name = "params";
					extraParams->value = entity.extraParams[i];
					node->addAttribute(extraParams);
				}
			}
		}
	}

protected:
	AbstractWriter* writer;
};
