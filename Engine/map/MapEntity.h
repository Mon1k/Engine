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
		LIGHT = 7
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
		std::vector<std::string> extraParams; // @todo - deprecation
		std::map<std::string, std::string> params;

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
	}

	std::vector<MapEntity::ObjectFormat>* getEntities()
	{
		return &m_entities;
	}

	void clear()
	{
		m_entities.clear();
	}

	Model* copyModel(MapEntity::ObjectFormat entity, ModelManager* manager)
	{
		Model* model = new Model;
		Model* modelOriginal = dynamic_cast<Model*>(manager->getById(entity.id));
		D3DXVECTOR3 position;

		position = entity.position;
		float maxX = modelOriginal->getMaxPosition().x;
		float minX = modelOriginal->getMinPosition().x;
		position.x += (maxX - minX) * 1.1;

		model->Initialize(modelOriginal->getD3D(), &entity.path[0], {entity.texture});
		model->SetPosition(position);
		model->SetScale(entity.scale);
		model->SetRotation(entity.rotation);
		model->setAlpha(modelOriginal->getAlpha());
		model->addLights(modelOriginal->getLights());

		LightShaderClass* shader = new LightShaderClass;
		shader->Initialize(modelOriginal->getD3D()->GetDevice());
		shader->addLights(modelOriginal->getLights());
		model->addShader(shader);

		model->setId(manager->getNextId());
		manager->Add(model);

		MapEntity::ObjectFormat entityNew = entity;
		entityNew.id = model->getId();
		entityNew.position = model->GetPosition();
		this->add(entityNew);

		return model;
	}
};
