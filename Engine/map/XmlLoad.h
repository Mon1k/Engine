#pragma once

#include "../tool/String.h"

#include "../reader/AbstractReader.h"
#include "../reader/xml/Node.h"
#include "MapEntity.h"

#include "../models/ModelManager.h"
#include "../models/WaterNode.h"
#include "../models/terrain/terrainclass.h"
#include "../models/sky/skydomeclass.h"

class XmlLoad
{
public:
	XmlLoad(AbstractReader* reader)
	{
		this->reader = reader;

		//// @todo - later add lights in editor
		m_lightModel = new LightClass;
		m_lightModel->SetAmbientColor(0.35f, 0.35f, 0.35f, 1.0f);
		m_lightModel->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_lightModel->SetDirection(0.0f, -1.0f, 1.0f);
		m_lightModel->SetPosition(100.0f, 80.0f, -80.0f);
		m_lightModel->SetLookAt(-100.0f, 0.0f, 50.0f);
		m_lightModel->setIntensity(1.0f);
		m_lightModel->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_lightModel->SetSpecularPower(128.0f);
		m_lightModel->setType(LightClass::LightType::LIGHT_DIRECTIONAL);

		m_lightModel->GenerateProjectionMatrix(Options::shadow_depth, Options::shadow_near);
		m_lightModel->GenerateOrthoMatrix(Options::shadow_width, Options::screen_depth, Options::screen_near);

		m_light = new LightClass;
		m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
		m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_light->SetSpecularPower(32.0f);
		m_light->setIntensity(1.0f);
		////
	}

	void load(std::string path, MapEntity* entities, ModelManager* manager)
	{
		Node* node = static_cast<Node*>(this->reader->read(path));

		manager->clear();
		entities->clear();

		loadEntities(node, entities, manager);
	}

protected:
	void loadEntities(Node* node, MapEntity* entities, ModelManager* manager)
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
				loadEntities(node->childs[i], entities, manager);
			}
		}
	}

	void loadModel(Node* node, MapEntity* entities, ModelManager* manager)
	{
		ModelClass* model = new ModelClass;
		std::string path, texture;
		D3DXVECTOR3 position, scale, rotation;
		std::map<std::string, std::string> params;
		std::vector<std::string> textures;
		std::string name;
		int id;

		std::vector<Attribute*> attributes = node->getAttributes();
		for (size_t i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == "id") {
				id = stoi(attributes[i]->value);
			}
			else if (attributes[i]->name == "name") {
				name = attributes[i]->value;
			}
			else if (attributes[i]->name == "path") {
				path = attributes[i]->value;
			}
			else if (attributes[i]->name == "texture") {
				texture = attributes[i]->value;
				textures.push_back(texture);
			}
			else if (attributes[i]->name == "position") {
				position = extractVector3(attributes[i]);
			}
			else if (attributes[i]->name == "scale") {
				scale = extractVector3(attributes[i]);
			}
			else if (attributes[i]->name == "rotation") {
				rotation = extractVector3(attributes[i]);
			}
			else {
				params.insert(std::pair<std::string, std::string>(attributes[i]->name, attributes[i]->value));
			}
		}

		std::vector<Attribute*> extraTextures = node->getAttributes("extra_textures");
		for (size_t i = 0; i < extraTextures.size(); i++) {
			textures.push_back(extraTextures[i]->value);
		}

		model->addLights({ m_lightModel });
		LightShaderClass* shader = new LightShaderClass;
		shader->Initialize(manager->getD3D()->GetDevice());
		shader->addLights({ m_lightModel });
		model->addShader(shader);

		bool result = model->Initialize(manager->getD3D(), &path[0], textures);
		if (result) {
			model->setId(id);
			model->SetScale(scale);
			model->SetPosition(position);
			model->SetRotation(rotation);
			
			if (params.find("alpha") != params.end()) {
				model->setAlpha((bool)std::stoi(params["alpha"]));
			}
			if (params.find("shadow") != params.end()) {
				model->setShadow((bool)std::stoi(params["shadow"]));
			}
			

			manager->Add(model);

			MapEntity::ObjectFormat format;
			format.parent = 0;
			format.model = model;

			format.id = model->getId();
			format.name = name;
			format.type = MapEntity::ObjectTypes::MODEL;
			format.position = position;
			format.scale = scale;
			format.rotation = rotation;
			format.path = path;
			format.texture = texture;
			format.params = params;

			for (size_t i = 1; i < textures.size(); i++) {
				format.extraTextures.push_back(textures[i]);
			}
			
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
		entities->add(format);
	}

	void loadTerrain(Node* node, MapEntity* entities, ModelManager* manager)
	{
		TerrainClass* model = new TerrainClass;
		std::string path, texture, textureNormal;
		D3DXVECTOR3 position, scale, scaleNormal;
		std::map<std::string, std::string> params;
		std::string name;
		int id;

		std::vector<Attribute*> attributes = node->getAttributes();
		for (size_t i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == "id") {
				id = stoi(attributes[i]->value);
			}
			else if (attributes[i]->name == "name") {
				name = attributes[i]->value;
			}
			else if (attributes[i]->name == "path") {
				path = attributes[i]->value;
			}
			else if (attributes[i]->name == "texture") {
				texture = attributes[i]->value;
			}
			else if (attributes[i]->name == "position") {
				position = extractVector3(attributes[i]);
			}
			else if (attributes[i]->name == "scale") {
				scale = extractVector3(attributes[i]);
			}
			else if (attributes[i]->name == "scale_normal") {
				scaleNormal = extractVector3(attributes[i]);
				params.insert(std::pair<std::string, std::string>(attributes[i]->name, attributes[i]->value));
			}
			else {
				params.insert(std::pair<std::string, std::string>(attributes[i]->name, attributes[i]->value));
			}
		}


		model->SetScale(scale);
		model->SetPosition(position);
		model->setScaleNormal(scaleNormal);

		bool result = model->Initialize(manager->getD3D(), manager->getFrustum(), &path[0], texture, params["texture_normal"]);
		if (result) {
			model->setId(id);
			model->addLights({ m_lightModel });
			manager->Add(model);

			if (params["layer_alpha"].size() > 1 && params["layer1"].size() > 1 && params["layer1_normal"].size() > 1) {
				model->addTextureLayer(params["layer1"], params["layer1_normal"]);
				if (params["layer2"].size() > 1 && params["layer2_normal"].size() > 1) {
					model->addTextureLayer(params["layer2"], params["layer2_normal"]);
					if (params["layer3"].size() > 1 && params["layer3_normal"].size() > 1) {
						model->addTextureLayer(params["layer3"], params["layer3_normal"]);
					}
				}
				model->addTextureAlpha(params["layer_alpha"]);
			}

			MapEntity::ObjectFormat format;
			format.parent = 0;
			format.model = model;

			format.id = model->getId();
			format.name = name;
			format.type = MapEntity::ObjectTypes::TERRAIN;
			format.position = position;
			format.scale = scale;
			format.rotation = model->getRotation();
			format.path = path;
			format.texture = texture;
			format.params = params;

			entities->add(format);
		}
	}

	void loadWater(Node* node, MapEntity* entities, ModelManager* manager)
	{
		WaterNode* model = new WaterNode;
		std::string path, texture;
		D3DXVECTOR3 position, scale, rotation;
		D3DXVECTOR2 tiling;
		std::map<std::string, std::string> params;
		std::string name;
		int id;

		std::vector<Attribute*> attributes = node->getAttributes();
		for (size_t i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == "id") {
				id = std::stoi(attributes[i]->value);
			}
			else if (attributes[i]->name == "name") {
				name = attributes[i]->value;
			}
			else if (attributes[i]->name == "path") {
				path = attributes[i]->value;
			}
			else if (attributes[i]->name == "texture") {
				texture = attributes[i]->value;
			}
			else if (attributes[i]->name == "position") {
				position = extractVector3(attributes[i]);
			}
			else if (attributes[i]->name == "scale") {
				scale = extractVector3(attributes[i]);
			}
			else if (attributes[i]->name == "rotation") {
				rotation = extractVector3(attributes[i]);
			}
			else {
				params.insert(std::pair<std::string, std::string>(attributes[i]->name, attributes[i]->value));
			}
		}

		bool result = model->Initialize(manager->getD3D(), &path[0], { texture });
		if (result) {
			model->SetScale(scale);
			model->SetPosition(position);
			//model->SetRotation(rotation);
			model->setId(id);
			model->addLights({ m_light });
			model->setWaterHeight(std::stof(params["height"]));
			tiling = extractVector2(params["tiling"]);
			model->setNormalMapTiling(tiling.x, tiling.y);
			model->setReflectRefractScale(std::stof(params["refraction_scale"]));

			// @todo later changes it, set target after load target in model manager
			if (params.find("targetId") != params.end()) {
				Model* modelTarget = dynamic_cast<Model*>(manager->getById(std::stoi(params["targetId"])));
				if (modelTarget) {
					model->addRefractionTarget(modelTarget);
				}
			}


			manager->Add(model);

			MapEntity::ObjectFormat format;
			format.parent = 0;
			format.model = model;

			format.id = model->getId();
			format.name = name;
			format.type = MapEntity::ObjectTypes::WATER;
			format.position = position;
			format.scale = scale;
			format.rotation = rotation;
			format.path = path;
			format.texture = texture;
			format.params = params;

			entities->add(format);
		}
	}

	void loadSky(Node* node, MapEntity* entities, ModelManager* manager)
	{
		SkyDomeClass* model = new SkyDomeClass;
		std::string path, texture;
		std::map<std::string, std::string> params;
		std::string name;
		int id;

		std::vector<Attribute*> attributes = node->getAttributes();
		for (size_t i = 0; i < attributes.size(); i++) {
			if (attributes[i]->name == "id") {
				id = std::stoi(attributes[i]->value);
			}
			else if (attributes[i]->name == "name") {
				name = attributes[i]->value;
			}
			else if (attributes[i]->name == "path") {
				path = attributes[i]->value;
			}
			else if (attributes[i]->name == "texture") {
				texture = attributes[i]->value;
			}
			else {
				params.insert(std::pair<std::string, std::string>(attributes[i]->name, attributes[i]->value));
			}
		}

		model->Initialize(manager->getD3D(), path);
		model->setId(id);

		manager->addUnshift(model);

		MapEntity::ObjectFormat format;
		format.parent = 0;
		format.model = model;

		format.id = id;
		format.name = name;
		format.type = MapEntity::ObjectTypes::SKY;
		format.path = path;

		if (params.find("apex_color") != params.end()) {
			model->setApexColor(extractVector4(params["apex_color"]));
		}
		if (params.find("center_color") != params.end()) {
			model->setCenterColor(extractVector4(params["center_color"]));
		}
			
		entities->add(format);
	}

	D3DXVECTOR2 extractVector2(Attribute* attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute->value);

		return D3DXVECTOR2(std::stof(arrayAttributes[0]), std::stof(arrayAttributes[1]));
	}

	D3DXVECTOR2 extractVector2(std::string attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute);

		return D3DXVECTOR2(std::stof(arrayAttributes[0]), std::stof(arrayAttributes[1]));
	}

	D3DXVECTOR3 extractVector3(Attribute* attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute->value);

		return D3DXVECTOR3(std::stof(arrayAttributes[0]), std::stof(arrayAttributes[1]), std::stof(arrayAttributes[2]));
	}

	D3DXVECTOR3 extractVector3(std::string attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute);

		return D3DXVECTOR3(std::stof(arrayAttributes[0]), std::stof(arrayAttributes[1]), std::stof(arrayAttributes[2]));
	}

	D3DXVECTOR4 extractVector4(Attribute* attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute->value);

		return D3DXVECTOR4(std::stof(arrayAttributes[0]), std::stof(arrayAttributes[1]), std::stof(arrayAttributes[2]), std::stof(arrayAttributes[3]));
	}

	D3DXVECTOR4 extractVector4(std::string attribute)
	{
		std::vector<std::string> arrayAttributes = String::explode(";", attribute);

		return D3DXVECTOR4(std::stof(arrayAttributes[0]), std::stof(arrayAttributes[1]), std::stof(arrayAttributes[2]), std::stof(arrayAttributes[3]));
	}


public:
	AbstractReader* reader;

	LightClass* m_lightModel;
	LightClass* m_light;
};