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
	};
};
