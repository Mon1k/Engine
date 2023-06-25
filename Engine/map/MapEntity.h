#pragma once

#include <string>
#include <vector>

#include <d3dx10math.h>

#include "../models/CompositeModel.h"
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
		COMPOSITE_MODEL = 5
	};

	struct ObjectFormat {
		int id;
		std::string name;
		D3DXVECTOR3 position;
		D3DXVECTOR3 scale;
		D3DXVECTOR3 rotation;
		std::string path;
		std::string texture;
		std::vector<std::string> extraParams;
		ObjectTypes type;
		CompositeModel* parent;
	} format;

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

	MapEntity::ObjectFormat* getEntity(int id)
	{
		int size = m_entities.size();
		for (int i = 0; i < size; i++) {
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

	ModelClass* copyModel(MapEntity::ObjectFormat entity, ModelManager* manager)
	{
		ModelClass* model = new ModelClass;
		ModelClass* modelOriginal = dynamic_cast<ModelClass*>(manager->getById(entity.id));
		D3DXVECTOR3 position;

		position = entity.position;
		float maxX = modelOriginal->getMaxPosition().x;
		float minX = modelOriginal->getMinPosition().x;
		float width = maxX - minX;
		position.x += width * 1.1;

		model->Initialize(modelOriginal->getD3D(), &entity.path[0], {entity.texture});
		model->SetPosition(position);
		model->SetScale(entity.scale);
		model->SetRotation(entity.rotation);
		model->addLights({ modelOriginal->getLight(0) });

		LightShaderClass* shader = new LightShaderClass;
		shader->Initialize(modelOriginal->getD3D()->GetDevice());
		shader->addLights({ modelOriginal->getLight(0) });
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
