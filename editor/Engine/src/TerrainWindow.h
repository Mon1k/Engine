#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/FileInput.h"

#include "../../Engine/models/terrain/terrainclass.h"

class TerrainWindow
{
public:
	TerrainWindow::TerrainWindow(App* app)
	{
		m_app = app;
		resetUI();
	}

	void resetUI()
	{
		m_path = "data/textures/heightmap.bmp";
		m_texture = "data/textures/grass.dds";
		m_textureNormal = "data/textures/grass_n.dds";

		m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		m_scaleNormal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		m_textureAlpha = "";
		m_textureLayer1 = "";
		m_textureNormalLayer1 = "";
		m_textureLayer2 = "";
		m_textureNormalLayer2 = "";
		m_textureLayer3 = "";
		m_textureNormalLayer3 = "";
	}

	void initialize()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_app->m_uiManager->getById(1));

		m_Window = new Window;
		m_app->m_uiManager->add(m_Window);
		m_Window->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		m_Window->addHeader("Terrain properties", Window::HEADER_BUTTON_CLOSE);
		m_Window->addBody();
		m_Window->hide();
		m_Window->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			m_app->unselectModel();
		});
		shift += m_Window->m_y + m_Window->getHeader()->m_height + 5;

		FileInput* objectPath = new FileInput;
		m_Window->addChild(objectPath);
		objectPath->initialize(400, 28, m_Window->m_x + 10, shift);
		objectPath->getDialog()->setPath(objectPath->getDialog()->getCurrentPath() + "data/textures");
		objectPath->getDialog()->addFilter("bmp");
		objectPath->setValueRefLink(&typeid(std::string), &m_path);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			if (!m_app->m_selectedModel) {
				TerrainClass* model = new TerrainClass;
				model->setId(m_app->m_modelManager->getNextId());
				model->addLights({ m_app->m_light });

				m_app->m_modelManager->Add(model);
				m_app->m_selectedModel = model;

				MapEntity::ObjectFormat format;
				format.model = model;
				format.parent = 0;

				format.id = model->getId();
				format.name = "Terrain " + std::to_string(format.id);
				format.type = MapEntity::ObjectTypes::TERRAIN;
				format.position = model->GetPosition();
				format.scale = model->GetScale();
				format.rotation = model->getRotation();
				format.path = m_path;
				format.texture = m_texture;
					
				format.params.insert(std::pair<std::string, std::string>("texture_normal", m_textureNormal));
				format.params.insert(std::pair<std::string, std::string>("scale_normal", "1.0;1.0;1.0"));
				format.params.insert(std::pair<std::string, std::string>("layer_alpha", ""));
				format.params.insert(std::pair<std::string, std::string>("layer1", ""));
				format.params.insert(std::pair<std::string, std::string>("layer1_normal", ""));
				format.params.insert(std::pair<std::string, std::string>("layer2", ""));
				format.params.insert(std::pair<std::string, std::string>("layer2_normal", ""));
				format.params.insert(std::pair<std::string, std::string>("layer3", ""));
				format.params.insert(std::pair<std::string, std::string>("layer3_normal", ""));

				m_app->m_mapEntities->add(format);
			}
			this->updateTerrain();
		});
		shift += objectPath->m_height + 5;


		FileInput* objectTexture = new FileInput;
		m_Window->addChild(objectTexture);
		objectTexture->initialize(400, 28, m_Window->m_x + 10, shift);
		objectTexture->getDialog()->setPath(objectTexture->getDialog()->getCurrentPath() + "data/textures");
		objectTexture->getDialog()->addDefaultImageFilters();
		objectTexture->setValueRefLink(&typeid(std::string), &m_texture);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		shift += objectTexture->m_height + 5;

		FileInput* objectTextureNormal = new FileInput;
		m_Window->addChild(objectTextureNormal);
		objectTextureNormal->initialize( 400, 28, m_Window->m_x + 10, shift);
		objectTextureNormal->getDialog()->setPath(objectTextureNormal->getDialog()->getCurrentPath() + "data/textures");
		objectTextureNormal->getDialog()->addDefaultImageFilters();
		objectTextureNormal->setValueRefLink(&typeid(std::string), &m_textureNormal);
		objectTextureNormal->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		shift += objectTextureNormal->m_height + 5;


		Label* objectWindowLabelPosition = new Label;
		m_Window->addChild(objectWindowLabelPosition);
		objectWindowLabelPosition->Initialize(50, 20);
		objectWindowLabelPosition->Add("Position", m_Window->m_x + 5, shift);
		shift += objectWindowLabelPosition->m_height + 1;

		Input* objectPositionX = new Input;
		m_Window->addChild(objectPositionX);
		objectPositionX->initialize(100, 28, m_Window->m_x + 10, shift);
		objectPositionX->setValueRefLink(&typeid(float), &m_position.x);
		objectPositionX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		Input* objectPositionY = new Input;
		m_Window->addChild(objectPositionY);
		objectPositionY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectPositionY->setValueRefLink(&typeid(float), &m_position.y);
		objectPositionY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		Input* objectPositionZ = new Input;
		m_Window->addChild(objectPositionZ);
		objectPositionZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectPositionZ->setValueRefLink(&typeid(float), &m_position.z);
		objectPositionZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		shift += objectPositionX->m_height + 5;


		Label* objectWindowLabelScale = new Label;
		m_Window->addChild(objectWindowLabelScale);
		objectWindowLabelScale->Initialize(50, 20);
		objectWindowLabelScale->Add("Scale", m_Window->m_x + 5, shift);
		shift += objectWindowLabelScale->m_height + 1;

		Input* objectScaleX = new Input;
		m_Window->addChild(objectScaleX);
		objectScaleX->initialize(100, 28, m_Window->m_x + 10, shift);
		objectScaleX->setValueRefLink(&typeid(float), &m_scale.x);
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		Input* objectScaleY = new Input;
		m_Window->addChild(objectScaleY);
		objectScaleY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectScaleY->setValueRefLink(&typeid(float), &m_scale.y);
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		Input* objectScaleZ = new Input;
		m_Window->addChild(objectScaleZ);
		objectScaleZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectScaleZ->setValueRefLink(&typeid(float), &m_scale.z);
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		shift += objectScaleX->m_height + 5;


		Label* objectWindowLabelScaleNormal = new Label;
		m_Window->addChild(objectWindowLabelScaleNormal);
		objectWindowLabelScaleNormal->Initialize(50, 20);
		objectWindowLabelScaleNormal->Add("Scale normal", m_Window->m_x + 5, shift);
		shift += objectWindowLabelScaleNormal->m_height + 1;

		Input* objectScaleNormalX = new Input;
		m_Window->addChild(objectScaleNormalX);
		objectScaleNormalX->initialize(100, 28, m_Window->m_x + 10, shift);
		objectScaleNormalX->setValueRefLink(&typeid(float), &m_scaleNormal.x);
		objectScaleNormalX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		Input* objectScaleNormalY = new Input;
		m_Window->addChild(objectScaleNormalY);
		objectScaleNormalY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectScaleNormalY->setValueRefLink(&typeid(float), &m_scaleNormal.y);
		objectScaleNormalY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			int x = 1;
			this->updateTerrain();
		});
		Input* objectScaleNormalZ = new Input;
		m_Window->addChild(objectScaleNormalZ);
		objectScaleNormalZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectScaleNormalZ->setValueRefLink(&typeid(float), &m_scaleNormal.z);
		objectScaleNormalZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		shift += objectScaleNormalX->m_height + 5;


		// layers
		Label* objectWindowLabeLayers = new Label;
		m_Window->addChild(objectWindowLabeLayers);
		objectWindowLabeLayers->Initialize(50, 20);
		objectWindowLabeLayers->Add("Layers", m_Window->m_x + 5, shift);
		shift += objectWindowLabeLayers->m_height + 1;

		FileInput* objectLayerAlpha = new FileInput;
		m_Window->addChild(objectLayerAlpha);
		objectLayerAlpha->initialize(400, 28, m_Window->m_x + 10, shift);
		objectLayerAlpha->getDialog()->setPath(objectLayerAlpha->getDialog()->getCurrentPath() + "data/textures");
		objectLayerAlpha->getDialog()->addDefaultImageFilters();
		objectLayerAlpha->setValueRefLink(&typeid(std::string), &m_textureAlpha);
		objectLayerAlpha->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateTerrain();
		});
		shift += objectLayerAlpha->m_height + 5;

		FileInput* objectLayer1 = new FileInput;
		m_Window->addChild(objectLayer1);
		objectLayer1->initialize(200, 28, m_Window->m_x + 10, shift);
		objectLayer1->getDialog()->setPath(objectLayer1->getDialog()->getCurrentPath() + "data/textures");
		objectLayer1->getDialog()->addDefaultImageFilters();
		objectLayer1->setValueRefLink(&typeid(std::string), &m_textureLayer1);
		FileInput* objectLayer1Normal = new FileInput;
		m_Window->addChild(objectLayer1Normal);
		objectLayer1Normal->initialize(200, 28, m_Window->m_x + 212, shift);
		objectLayer1Normal->getDialog()->setPath(objectLayer1Normal->getDialog()->getCurrentPath() + "data/textures");
		objectLayer1Normal->getDialog()->addDefaultImageFilters();
		objectLayer1Normal->setValueRefLink(&typeid(std::string), &m_textureNormalLayer1);
		shift += objectLayer1->m_height + 5;

		FileInput* objectLayer2 = new FileInput;
		m_Window->addChild(objectLayer2);
		objectLayer2->initialize(200, 28, m_Window->m_x + 10, shift);
		objectLayer2->getDialog()->setPath(objectLayer2->getDialog()->getCurrentPath() + "data/textures");
		objectLayer2->getDialog()->addDefaultImageFilters();
		objectLayer2->setValueRefLink(&typeid(std::string), &m_textureLayer2);
		FileInput* objectLayer2Normal = new FileInput;
		m_Window->addChild(objectLayer2Normal);
		objectLayer2Normal->initialize(200, 28, m_Window->m_x + 212, shift);
		objectLayer2Normal->getDialog()->setPath(objectLayer2Normal->getDialog()->getCurrentPath() + "data/textures");
		objectLayer2Normal->getDialog()->addDefaultImageFilters();
		objectLayer2Normal->setValueRefLink(&typeid(std::string), &m_textureNormalLayer2);
		shift += objectLayer2->m_height + 5;

		FileInput* objectLayer3 = new FileInput;
		m_Window->addChild(objectLayer3);
		objectLayer3->initialize(200, 28, m_Window->m_x + 10, shift);
		objectLayer3->getDialog()->setPath(objectLayer3->getDialog()->getCurrentPath() + "data/textures");
		objectLayer3->getDialog()->addDefaultImageFilters();
		objectLayer3->setValueRefLink(&typeid(std::string), &m_textureLayer3);
		FileInput* objectLayer3Normal = new FileInput;
		m_Window->addChild(objectLayer3Normal);
		objectLayer3Normal->initialize(200, 28, m_Window->m_x + 212, shift);
		objectLayer3Normal->getDialog()->setPath(objectLayer3Normal->getDialog()->getCurrentPath() + "data/textures");
		objectLayer3Normal->getDialog()->addDefaultImageFilters();
		objectLayer3Normal->setValueRefLink(&typeid(std::string), &m_textureNormalLayer3);
		shift += objectLayer3->m_height + 5;
	}

	void updateUiFromModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		TerrainClass* terrain = dynamic_cast<TerrainClass*>(m_app->m_selectedModel);
		m_path = terrain->getPath();
		m_texture = terrain->GetTextureClass()->getTexturePath();
		m_position = terrain->GetPosition();
		m_scale = terrain->getOrigScale();
		m_scaleNormal = terrain->getScaleNormal();

		std::vector<std::string> textures = terrain->GetTextureClass()->getTexturesPath();
		if (textures.size() >= 5) {
			m_textureLayer1 = textures[2];
			m_textureNormalLayer1 = textures[3];
			m_textureAlpha = textures[4];

			if (textures.size() >= 7) {
				m_textureLayer2 = textures[4];
				m_textureNormalLayer2 = textures[5];
				m_textureAlpha = textures[6];

				if (textures.size() == 9) {
					m_textureLayer3 = textures[6];
					m_textureNormalLayer3 = textures[7];
					m_textureAlpha = textures[8];
				}
			}
		}

		m_Window->setTitle("Terrain properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();
	}

	void updateTerrain()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		m_Window->setTitle("Terrain properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();

		TerrainClass* terrain = dynamic_cast<TerrainClass*>(m_app->m_selectedModel);
		terrain->Shutdown();
		terrain->SetPosition(m_position);
		terrain->SetScale(m_scale);
		terrain->setScaleNormal(m_scaleNormal);
		terrain->Initialize(m_app->getGraphic()->getD3D(), m_app->getGraphic()->getFrustum(), &m_path[0], m_texture, m_textureNormal);
		if (m_textureAlpha.length() > 1 && m_textureLayer1.length() > 1 && m_textureNormalLayer1.length() > 1) {
			terrain->addTextureLayer(m_textureLayer1, m_textureNormalLayer1);
			if (m_textureLayer2.length() > 1 && m_textureNormalLayer2.length() > 1) {
				terrain->addTextureLayer(m_textureLayer2, m_textureNormalLayer2);
				if (m_textureLayer3.length() > 1 && m_textureNormalLayer3.length() > 1) {
					terrain->addTextureLayer(m_textureLayer3, m_textureNormalLayer3);
				}
			}
			terrain->addTextureAlpha(m_textureAlpha);
		}
		m_app->m_selectedModel->refreshBBox();

		MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
		editorFormat->path = m_path;
		editorFormat->texture = m_texture;
		editorFormat->position = m_position;
		editorFormat->scale = m_scale;
		editorFormat->params["texture_normal"] = m_textureNormal;
		editorFormat->params["scale_normal"] = std::to_string(m_scaleNormal.x) + ";" + std::to_string(m_scaleNormal.y) + ";" + std::to_string(m_scaleNormal.z);
		editorFormat->params["layer_alpha"] = m_textureAlpha;
		editorFormat->params["layer1"] = m_textureLayer1;
		editorFormat->params["layer1_normal"] = m_textureNormalLayer1;
		editorFormat->params["layer2"] = m_textureLayer2;
		editorFormat->params["layer2_normal"] = m_textureNormalLayer2;
		editorFormat->params["layer3"] = m_textureLayer3;
		editorFormat->params["layer3_normal"] = m_textureNormalLayer3;
	}

	Window* getWindow()
	{
		return m_Window;
	}

protected:
	App* m_app;
	Window* m_Window;

	std::string m_path;
	std::string m_texture;
	std::string m_textureNormal;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_scale;
	D3DXVECTOR3 m_scaleNormal;

	std::string m_textureAlpha;
	std::string m_textureLayer1;
	std::string m_textureNormalLayer1;
	std::string m_textureLayer2;
	std::string m_textureNormalLayer2;
	std::string m_textureLayer3;
	std::string m_textureNormalLayer3;
};