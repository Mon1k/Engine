#pragma once

#include "../tool/String.h"

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

		//// @todo
		m_light = new LightClass;
		m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
		m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_light->SetDirection(0.0f, -1.0f, 1.0f);
		m_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_light->SetSpecularPower(32.0f);
		////
	}

	void load(std::string path, MapEntity* entities, ModelManager* manager)
	{
		Node* node = static_cast<Node*>(this->reader->read(path));

		manager->clear();
		entities->clear();

		loadEntity(node, entities, manager);
	}

protected:
	void loadEntity(Node* node, MapEntity* entities, ModelManager* manager)
	{
		if (node->name == "model") {
			loadModel(node, entities, manager);
		}
		if (node->name == "compositeModel") {
			loadCompositeModel(node, entities, manager);
		}
		if (node->name == "terrain") {
			loadTerrain(node, entities, manager);
		}
		if (node->name == "water") {
			loadWater(node, entities, manager);
		}
		if (node->name == "sky") {
			loadSky(node, entities, manager);
		}

		if (node->childs.size() > 0) {
			for (int i = 0; i < node->childs.size(); i++) {
				loadEntity(node->childs[i], entities, manager);
			}
		}
	}

	void loadModel(Node* node, MapEntity* entities, ModelManager* manager)
	{
		ModelClass* model = new ModelClass;
		std::string path, texture;
		D3DXVECTOR3 position, scale, rotation;

		int id = stoi(node->getAttribute("id")->value);

		path = node->getAttribute("path")->value;
		texture = node->getAttribute("texture")->value;

		position = extractVector3(node->getAttribute("position"));
		scale = extractVector3(node->getAttribute("scale"));
		rotation = extractVector3(node->getAttribute("rotation"));

		bool result = model->Initialize(manager->getD3D(), &path[0], {texture});
		if (result) {
			model->SetPosition(position);
			model->SetScale(scale);
			model->SetRotation(rotation);
			model->setId(id);

			model->addLights({ m_light });
			LightShaderClass* shader = new LightShaderClass;
			shader->Initialize(manager->getD3D()->GetDevice());
			shader->addLights({ m_light });
			model->addShader(shader);

			manager->Add(model);

			MapEntity::ObjectFormat format;
			format.id = model->getId();
			format.type = MapEntity::ObjectTypes::MODEL;
			format.position = position;
			format.scale = scale;
			format.rotation = rotation;
			format.path = path;
			format.texture = texture;
			format.parent = 0;
			entities->add(format);
		}
	}

	void loadCompositeModel(Node* node, MapEntity* entities, ModelManager* manager)
	{
		AbstractModel* model1;
		AbstractModel* model2;
		CompositeModel* model = new CompositeModel;
		D3DXVECTOR3 position, scale, rotation;
		std::vector<Attribute*> params;

		int id = stoi(node->getAttribute("id")->value);

		position = extractVector3(node->getAttribute("position"));
		scale = extractVector3(node->getAttribute("scale"));
		rotation = extractVector3(node->getAttribute("rotation"));

		params = node->getAttributes("params");


		//model->SetPosition(position);
		//model->SetScale(scale);
		//model->SetRotation(rotation);
		model->setId(id);

		model1 = manager->getById(stoi(params[0]->value));
		model2 = manager->getById(stoi(params[1]->value));
		model->addChild(model1);
		model->addChild(model2);
		manager->Add(model);

		MapEntity::ObjectFormat format;
		format.id = model->getId();
		format.type = MapEntity::ObjectTypes::COMPOSITE_MODEL;
		format.position = position;
		format.scale = scale;
		format.rotation = rotation;
		format.extraParams.push_back(params[0]->value);
		format.extraParams.push_back(params[1]->value);
		entities->add(format);
	}

	void loadTerrain(Node* node, MapEntity* entities, ModelManager* manager)
	{
		TerrainClass* model = new TerrainClass;
		std::string path, texture;
		D3DXVECTOR3 position, scale, scaleNormal;
		std::vector<Attribute*> params;

		int id = stoi(node->getAttribute("id")->value);

		path = node->getAttribute("path")->value;
		texture = node->getAttribute("texture")->value;

		params = node->getAttributes("params");

		position = extractVector3(node->getAttribute("position"));
		scale = extractVector3(node->getAttribute("scale"));

		model->SetPosition(position);
		model->SetScale(scale);

		bool result = model->Initialize(manager->getD3D(), manager->getFrustum(), &path[0], texture, params[0]->value);
		if (result) {
			model->setId(id);
			model->addLights({ m_light });
			manager->Add(model);

			MapEntity::ObjectFormat format;
			format.id = model->getId();
			format.type = MapEntity::ObjectTypes::TERRAIN;
			format.position = position;
			format.scale = scale;
			format.path = path;
			format.texture = texture;
			for (int i = 0; i <= 7; i++) {
				if (i < params.size()) {
					format.extraParams.push_back(params[i]->value);
				}
				else {
					format.extraParams.push_back("");
				}
			}
			entities->add(format);
		}
	}

	void loadWater(Node* node, MapEntity* entities, ModelManager* manager)
	{
		WaterNode* model = new WaterNode;
		std::string path, texture;
		D3DXVECTOR3 position, scale, rotation;
		std::vector<Attribute*> params;

		int id = stoi(node->getAttribute("id")->value);

		path = node->getAttribute("path")->value;
		texture = node->getAttribute("texture")->value;

		position = extractVector3(node->getAttribute("position"));
		scale = extractVector3(node->getAttribute("scale"));
		rotation = extractVector3(node->getAttribute("rotation"));

		params = node->getAttributes("params");

		bool result = model->Initialize(manager->getD3D(), &path[0], { texture });
		if (result) {
			model->SetPosition(position);
			model->SetScale(scale);
			//model->SetRotation(rotation);
			model->setId(id);
			model->addLights({ m_light });
			model->setWaterHeight(stof(params[0]->value));
			model->setNormalMapTiling(stof(params[1]->value), stof(params[2]->value));
			model->setReflectRefractScale(stof(params[3]->value));

			ModelClass* modelTarget = dynamic_cast<ModelClass*>(manager->getById(stoi(params[4]->value)));
			model->addRefractionTarget(modelTarget);

			manager->Add(model);

			MapEntity::ObjectFormat format;
			format.id = model->getId();
			format.type = MapEntity::ObjectTypes::WATER;
			format.position = position;
			format.scale = scale;
			format.rotation = rotation;
			format.path = path;
			format.texture = texture;
			for (int i = 0; i < params.size(); i++) {
				format.extraParams.push_back(params[i]->value);
			}
			entities->add(format);
		}
	}

	void loadSky(Node* node, MapEntity* entities, ModelManager* manager)
	{
		SkyPlaneClass* model = new SkyPlaneClass;
		SkyDomeClass* modelDome = new SkyDomeClass;
		std::string path, texture;
		D3DXVECTOR3 position, scale, rotation;
		std::vector<Attribute*> params;

		int id = stoi(node->getAttribute("id")->value);

		path = node->getAttribute("path")->value;
		texture = node->getAttribute("texture")->value;

		params = node->getAttributes("params");

		bool result = model->Initialize(manager->getD3D(), { texture, params[0]->value });
		if (result) {
			model->setId(id);

			modelDome->Initialize(manager->getD3D(), path);
			modelDome->setId(stoi(params[1]->value));

			manager->Add(modelDome);
			manager->Add(model);

			MapEntity::ObjectFormat format;
			format.id = model->getId();
			format.type = MapEntity::ObjectTypes::SKY;
			format.path = path;
			format.texture = texture;
			for (int i = 0; i < params.size(); i++) {
				format.extraParams.push_back(params[i]->value);
			}
			entities->add(format);
		}
	}

	D3DXVECTOR3 extractVector3(Attribute* attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute->value);

		return D3DXVECTOR3(stof(arrayAttributes[0]), stof(arrayAttributes[1]), stof(arrayAttributes[2]));
	}

public:
	AbstractReader* reader;

	LightClass* m_light;
};