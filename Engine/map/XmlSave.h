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

	void save(std::string path, MapEntity* entities)
	{
		this->writer->save(path, getNode(entities));
	}

	Node* getNode(MapEntity* entities)
	{
		Node* node = new Node();
		node->name = "world";

		getModelsNode(node, entities);

		return node;
	}

	void getModelsNode(Node* node, MapEntity* entities)
	{
		std::vector<MapEntity::ObjectFormat> entitiesList = entities->m_entities;

		for (size_t i = 0; i < entitiesList.size(); i++) {
			if (entitiesList[i].type == MapEntity::ObjectTypes::MODEL) {
				Node* child = new Node;

				child->name = "model";
				fillAttributes(child, entitiesList[i]);

				node->addChild(child);
			}
			if (entitiesList[i].type == MapEntity::ObjectTypes::COMPOSITE_MODEL) {
				Node* child = new Node;

				child->name = "compositeModel";
				fillAttributes(child, entitiesList[i]);

				node->addChild(child);
			}
			if (entitiesList[i].type == MapEntity::ObjectTypes::TERRAIN) {
				Node* child = new Node;

				child->name = "terrain";
				fillAttributes(child, entitiesList[i]);

				node->addChild(child);
			}
			if (entitiesList[i].type == MapEntity::ObjectTypes::WATER) {
				Node* child = new Node;

				child->name = "water";
				fillAttributes(child, entitiesList[i]);

				node->addChild(child);
			}
			if (entitiesList[i].type == MapEntity::ObjectTypes::SKY) {
				Node* child = new Node;

				child->name = "sky";
				fillAttributes(child, entitiesList[i]);

				node->addChild(child);
			}
		}
	}

	void fillAttributes(Node* node, MapEntity::ObjectFormat entity)
	{
		if (entity.id) {
			Attribute* id = new Attribute;
			id->name = "id";
			id->value = std::to_string(entity.id);
			node->addAttribute(id);
		}

		if (entity.name.length() > 0) {
			Attribute* name = new Attribute;
			name->name = "name";
			name->value = entity.name;
			node->addAttribute(name);
		}

		if (entity.type != 0) {
			Attribute* type = new Attribute;
			type->name = "type";
			type->value = std::to_string(entity.type);
			node->addAttribute(type);
		}

		if (entity.position) {
			Attribute* position = new Attribute;
			position->name = "position";
			position->value = std::to_string(entity.position.x) + ";" + std::to_string(entity.position.y) + ";" + std::to_string(entity.position.z);
			node->addAttribute(position);
		}

		if (entity.scale) {
			Attribute* scale = new Attribute;
			scale->name = "scale";
			scale->value = std::to_string(entity.scale.x) + ";" + std::to_string(entity.scale.y) + ";" + std::to_string(entity.scale.z);
			node->addAttribute(scale);
		}

		if (entity.rotation) {
			Attribute* rotation = new Attribute;
			rotation->name = "rotation";
			rotation->value = std::to_string(entity.rotation.x) + ";" + std::to_string(entity.rotation.y) + ";" + std::to_string(entity.rotation.z);
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

		if (entity.params.size() > 0) {
			for (const auto& [key, value] : entity.params) {
				if (key == "extra_textures") {
					continue;
				}
				Attribute* params = new Attribute;
				params->name = key;
				params->value = value;
				node->addAttribute(params);
			}
		}

		if (entity.extraTextures.size() > 0) {
			for (size_t i = 0; i < entity.extraTextures.size(); i++) {
				if (entity.extraTextures[i].size() > 0) {
					Attribute* extraTextures = new Attribute;
					extraTextures->name = "extra_textures";
					extraTextures->value = entity.extraTextures[i];
					node->addAttribute(extraTextures);
				}
			}
		}
	}

protected:
	AbstractWriter* writer;
};
