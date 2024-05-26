#pragma once

#include <string>
#include <vector>
#include <map>

#include <d3dx10math.h>

#include "../models/CompositeModel.h"
#include "../modelclass.h"
#include "../models/ModelManager.h"
#include "../lightshaderclass.h"

class MapEntity
{
public:
	enum ObjectTypes : int {
		MODEL = 1,
		TERRAIN = 2,
		WATER = 3,
		SKY = 4,
		COMPOSITE_MODEL = 5,
		ACTOR = 6,
		LIGHT = 7,
		CAMERA = 8
	};

	struct ObjectFormat {
		ObjectTypes type;
		int id;
		std::string name;
		D3DXVECTOR3 position;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 rotation;
		std::string path;
		std::string texture;
		std::map<std::string, std::string> params;
		std::vector<std::string> extraTextures;

		ModelClass* model;
		MapEntity::ObjectFormat* parent;
		std::vector<MapEntity::ObjectFormat*> childs;
	};

	std::vector<MapEntity::ObjectFormat> m_entities;

public:
	MapEntity()
	{
		m_entities.clear();
	}

	void add(MapEntity::ObjectFormat entity)
	{
		m_entities.push_back(entity);
	}

	void remove(int id)
	{
		for (int i = 0; i < m_entities.size(); i++) {
			if (m_entities[i].id == id) {
				m_entities.erase(m_entities.begin() + i);
				return;
			}
		}
	}

	MapEntity::ObjectFormat* getEntity(int id)
	{
		for (int i = 0; i < m_entities.size(); i++) {
			if (m_entities[i].id == id) {
				return &m_entities[i];
			}
		}

		return nullptr;
	}

	std::vector<MapEntity::ObjectFormat>* getEntities()
	{
		return &m_entities;
	}

	void clear()
	{
		m_entities.clear();
	}

	ModelClass* copyModel(MapEntity::ObjectFormat entity, ModelManager* manager)
	{
		ModelClass* model = new ModelClass;
		ModelClass* modelOriginal = dynamic_cast<ModelClass*>(manager->getById(entity.id));
		D3DXVECTOR3 position;

		position = entity.position;
		float maxX = modelOriginal->getMaxPosition().x;
		float minX = modelOriginal->getMinPosition().x;
		position.x += (maxX - minX) * 1.1;

		LightShaderClass* shader = new LightShaderClass;
		shader->Initialize(modelOriginal->getD3D()->GetDevice());
		shader->addLights(modelOriginal->getLights());
		model->addShader(shader);

		std::vector<std::string> textures;
		textures.push_back(entity.texture);
		for (size_t i = 0; i < entity.extraTextures.size(); i++) {
			textures.push_back(entity.extraTextures[i]);
		}

		model->Initialize(modelOriginal->getD3D(), &entity.path[0], textures);
		model->SetScale(entity.scale);
		model->SetRotation(entity.rotation);
		model->SetPosition(position);
		model->setAlpha(modelOriginal->getAlpha());
		model->setShadow(modelOriginal->isShadow());
		model->addLights(modelOriginal->getLights());
		
		model->setId(manager->getNextId());
		manager->Add(model);

		MapEntity::ObjectFormat entityNew = entity;
		entityNew.id = model->getId();
		entityNew.name = "Object" + std::to_string(entityNew.id);
		entityNew.position = model->GetPosition();
		this->add(entityNew);

		return model;
	}
};
