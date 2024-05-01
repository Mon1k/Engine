#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/checkbox.h"
#include "../../../Engine/ui/FileInput.h"

class TerrainWindow
{
public:
	TerrainWindow::TerrainWindow(App* app)
	{
		m_app = app;
	}

	void resetUI()
	{
		m_path = "data/models/heightmap.bmp";
		m_texture = "data/textures/grass.dds";
		m_textureNormal = "data/textures/grass_n.dds";

		m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		m_scaleNormal = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}

	void initialize()
	{
		resetUI();

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

		Input* objectPath = new Input;
		m_Window->addChild(objectPath);
		objectPath->initialize(400, 28, m_Window->m_x + 10, shift);
		objectPath->setText("data/textures/heightmap.bmp");
		objectPath->setId(22);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			Input* objectPath = dynamic_cast<Input*>(m_app->m_uiManager->getById(22));
			if (!m_app->m_selectedModel) {
				Input* objectTexture = dynamic_cast<Input*>(m_app->m_uiManager->getById(23));
				Input* objectTextureNormal = dynamic_cast<Input*>(m_app->m_uiManager->getById(30));

				TerrainClass* model = new TerrainClass;
				bool result = model->Initialize(m_app->getGraphic()->getD3D(), m_app->getGraphic()->getFrustum(), &objectPath->getValue()[0], objectTexture->getValue(), objectTextureNormal->getValue());
				if (result) {
					model->setId(m_app->m_modelManager->getNextId());
					model->addLights({ m_app->m_light });

					m_app->m_modelManager->Add(model);
					m_app->m_selectedModel = model;

					m_Window->show();

					MapEntity::ObjectFormat format;
					format.id = model->getId();
					format.type = MapEntity::ObjectTypes::TERRAIN;
					format.position = model->GetPosition();
					format.scale = model->GetScale();
					format.rotation = model->getRotation();
					format.path = objectPath->getValue();
					format.texture = objectTexture->getValue();
					format.extraParams.push_back(objectTextureNormal->getValue());
					// scale normal
					format.extraParams.push_back("1.0;1.0;1.0");
					// add layers
					// alpha
					format.extraParams.push_back("");
					// layer1
					format.extraParams.push_back("");
					format.extraParams.push_back("");
					// layer2
					format.extraParams.push_back("");
					format.extraParams.push_back("");
					// layer3
					format.extraParams.push_back("");
					format.extraParams.push_back("");

					m_app->m_mapEntities->add(format);

					this->updateTerrain();
				}
			}
			else {
				MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
				editorFormat->path = objectPath->getValue();
				this->updateTerrain();
			}
			return 0;
			});
		shift += objectPath->m_height + 5;


		Input* objectTexture = new Input;
		m_Window->addChild(objectTexture);
		objectTexture->initialize(400, 28, m_Window->m_x + 10, shift);
		objectTexture->setText("data/textures/grass.dds");
		objectTexture->setId(23);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			if (m_app->m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
				editorFormat->texture = objectTexture->getValue();
				this->updateTerrain();
			}
			return 0;
			});
		shift += objectTexture->m_height + 5;

		Input* objectTextureNormal = new Input;
		m_Window->addChild(objectTextureNormal);
		objectTextureNormal->initialize( 400, 28, m_Window->m_x + 10, shift);
		objectTextureNormal->setText("data/textures/grass_n.dds");
		objectTextureNormal->setId(30);
		objectTextureNormal->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTextureNormal, this] {
			if (m_app->m_selectedModel) {
				this->updateTerrain();
			}
			return 0;
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
		objectPositionX->setId(24);
		objectPositionX->setText("0.00");
		objectPositionX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionX, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectPositionY = new Input;
		m_Window->addChild(objectPositionY);
		objectPositionY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectPositionY->setId(25);
		objectPositionY->setText("0.00");
		objectPositionY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionY, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectPositionZ = new Input;
		m_Window->addChild(objectPositionZ);
		objectPositionZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectPositionZ->setId(26);
		objectPositionZ->setText("0.00");
		objectPositionZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionZ, this] {
			this->updateTerrain();
			return 0;
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
		objectScaleX->setId(27);
		objectScaleX->setText("1.00");
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleX, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleY = new Input;
		m_Window->addChild(objectScaleY);
		objectScaleY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectScaleY->setId(28);
		objectScaleY->setText("1.00");
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleY, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleZ = new Input;
		m_Window->addChild(objectScaleZ);
		objectScaleZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectScaleZ->setId(29);
		objectScaleZ->setText("1.00");
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleZ, this] {
			this->updateTerrain();
			return 0;
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
		objectScaleNormalX->setId(31);
		objectScaleNormalX->setText("1.00");
		objectScaleNormalX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleNormalX, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleNormalY = new Input;
		m_Window->addChild(objectScaleNormalY);
		objectScaleNormalY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectScaleNormalY->setId(32);
		objectScaleNormalY->setText("1.00");
		objectScaleNormalY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleNormalY, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleNormalZ = new Input;
		m_Window->addChild(objectScaleNormalZ);
		objectScaleNormalZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectScaleNormalZ->setId(33);
		objectScaleNormalZ->setText("1.00");
		objectScaleNormalZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleNormalZ, this] {
			this->updateTerrain();
			return 0;
			});
		shift += objectScaleNormalX->m_height + 5;


		// layers
		Label* objectWindowLabeLayers = new Label;
		m_Window->addChild(objectWindowLabeLayers);
		objectWindowLabeLayers->Initialize(50, 20);
		objectWindowLabeLayers->Add("Layers", m_Window->m_x + 5, shift);
		shift += objectWindowLabeLayers->m_height + 1;

		Input* objectLayerAlpha = new Input;
		m_Window->addChild(objectLayerAlpha);
		objectLayerAlpha->initialize(400, 28, m_Window->m_x + 10, shift);
		objectLayerAlpha->setId(40);
		objectLayerAlpha->setText("");
		objectLayerAlpha->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectLayerAlpha, this] {
			this->updateTerrain();
			return 0;
			});
		shift += objectLayerAlpha->m_height + 5;

		Input* objectLayer1 = new Input;
		m_Window->addChild(objectLayer1);
		objectLayer1->initialize(200, 28, m_Window->m_x + 10, shift);
		objectLayer1->setId(34);
		objectLayer1->setText("");
		Input* objectLayer1Normal = new Input;
		m_Window->addChild(objectLayer1Normal);
		objectLayer1Normal->initialize(200, 28, m_Window->m_x + 212, shift);
		objectLayer1Normal->setId(35);
		objectLayer1Normal->setText("");
		shift += objectLayer1->m_height + 5;

		Input* objectLayer2 = new Input;
		m_Window->addChild(objectLayer2);
		objectLayer2->initialize(200, 28, m_Window->m_x + 10, shift);
		objectLayer2->setId(36);
		objectLayer2->setText("");
		Input* objectLayer2Normal = new Input;
		m_Window->addChild(objectLayer2Normal);
		objectLayer2Normal->initialize(200, 28, m_Window->m_x + 212, shift);
		objectLayer2Normal->setId(37);
		objectLayer2Normal->setText("");
		shift += objectLayer2->m_height + 5;

		Input* objectLayer3 = new Input;
		m_Window->addChild(objectLayer3);
		objectLayer3->initialize(200, 28, m_Window->m_x + 10, shift);
		objectLayer3->setId(38);
		objectLayer3->setText("");
		Input* objectLayer3Normal = new Input;
		m_Window->addChild(objectLayer3Normal);
		objectLayer3Normal->initialize(200, 28, m_Window->m_x + 212, shift);
		objectLayer3Normal->setId(39);
		objectLayer3Normal->setText("");
		shift += objectLayer3->m_height + 5;
	}

	void updateTerrain()
	{
		if (!m_app->m_selectedModel) {
			return;
		}
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