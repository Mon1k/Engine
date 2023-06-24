#pragma once

#include <string>
#include <vector>

#include <d3dx10math.h>

#include "../models/CompositeModel.h"

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
};
