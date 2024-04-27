#include "../../Engine/ui/UIManager.h"
#include "../../Engine/ui/button.h"
#include "../../Engine/ui/label.h"
#include "../../Engine/ui/cursor.h"
#include "../../Engine/ui/input.h"
#include "../../Engine/ui/window.h"
#include "../../Engine/ui/checkbox.h"

#include "../../Engine/models/ModelManager.h"
#include "../../Engine/modelclass.h"
#include "../../Engine/models/Model.h"
#include "../../Engine/models/terrain/terrainclass.h"
#include "../../Engine/models/WaterNode.h"
#include "../../Engine/models/sky/skydomeclass.h"
#include "../../Engine/models/sky/skyplaneclass.h"
#include "../../Engine/models/gridclass.h"

// editor ui
#include "src/MainWindow.h"
#include "src/ObjectWindow.h"

#include "../../Engine/lightclass.h"
#include "../../Engine/lightshaderclass.h"

#include "../../Engine/CollisionDetection.h"
#include "../../Engine/tool/String.h"

void App::initDefaultObjects()
{
	GridClass* grid = new GridClass;
	grid->Initialize(m_Graphics->getD3D(), 100, 100);
	grid->setId(1);
	m_modelManager->Add(grid);
}

void App::IninMenuTop()
{
	m_mainWindow = new MainWindow;
	m_mainWindow->m_app = this;
	m_mainWindow->initialize();

	m_objectUI = new ObjectWindow;
	m_objectUI->m_app = this;
	m_objectUI->initialize();
}

	/*void InitWindowTerrain()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_uiManager->getById(1));

		Window* terrainWindow = new Window;
		m_uiManager->Add(terrainWindow);
		terrainWindow->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		terrainWindow->addHeader("Terrain properties", Window::HEADER_BUTTON_CLOSE);
		terrainWindow->addBody();
		terrainWindow->hide();
		terrainWindow->setId(21);
		terrainWindow->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			unselectModel();
			return 0;
			});
		shift += terrainWindow->m_y + terrainWindow->getHeader()->m_height + 5;


		Input* objectPath = new Input;
		terrainWindow->addChild(objectPath);
		objectPath->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectPath->setText("data/textures/heightmap.bmp");
		objectPath->setId(22);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			Input* objectPath = dynamic_cast<Input*>(m_uiManager->getById(22));
			if (!m_selectedModel) {
				Input* objectTexture = dynamic_cast<Input*>(m_uiManager->getById(23));
				Input* objectTextureNormal = dynamic_cast<Input*>(m_uiManager->getById(30));

				TerrainClass* model = new TerrainClass;
				bool result = model->Initialize(m_Graphics->getD3D(), m_Graphics->getFrustum(), &objectPath->getValue()[0], objectTexture->getValue(), objectTextureNormal->getValue());
				if (result) {
					model->setId(m_modelManager->getNextId());
					model->addLights({ m_light });

					m_modelManager->Add(model);
					m_selectedModel = model;

					AbstractGui* objectWindow = m_uiManager->getById(21);
					objectWindow->show();
					objectWindow->focus();

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

					m_mapEntities->add(format);

					this->updateTerrain();
				}
			}
			else {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->path = objectPath->getValue();
				this->updateTerrain();
			}
			return 0;
			});
		shift += objectPath->m_height + 5;


		Input* objectTexture = new Input;
		terrainWindow->addChild(objectTexture);
		objectTexture->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectTexture->setText("data/textures/grass.dds");
		objectTexture->setId(23);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->texture = objectTexture->getValue();
				this->updateTerrain();
			}
			return 0;
			});
		shift += objectTexture->m_height + 5;

		Input* objectTextureNormal = new Input;
		terrainWindow->addChild(objectTextureNormal);
		objectTextureNormal->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectTextureNormal->setText("data/textures/grass_n.dds");
		objectTextureNormal->setId(30);
		objectTextureNormal->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTextureNormal, this] {
			if (m_selectedModel) {
				this->updateTerrain();
			}
			return 0;
			});
		shift += objectTextureNormal->m_height + 5;


		Label* objectWindowLabelPosition = new Label;
		terrainWindow->addChild(objectWindowLabelPosition);
		objectWindowLabelPosition->Initialize(50, 20);
		objectWindowLabelPosition->Add("Position", terrainWindow->m_x + 5, shift);
		shift += objectWindowLabelPosition->m_height + 1;

		Input* objectPositionX = new Input;
		terrainWindow->addChild(objectPositionX);
		objectPositionX->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectPositionX->setId(24);
		objectPositionX->setText("0.00");
		objectPositionX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionX, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectPositionY = new Input;
		terrainWindow->addChild(objectPositionY);
		objectPositionY->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 112, shift);
		objectPositionY->setId(25);
		objectPositionY->setText("0.00");
		objectPositionY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionY, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectPositionZ = new Input;
		terrainWindow->addChild(objectPositionZ);
		objectPositionZ->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 214, shift);
		objectPositionZ->setId(26);
		objectPositionZ->setText("0.00");
		objectPositionZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionZ, this] {
			this->updateTerrain();
			return 0;
			});
		shift += objectPositionX->m_height + 5;


		Label* objectWindowLabelScale = new Label;
		terrainWindow->addChild(objectWindowLabelScale);
		objectWindowLabelScale->Initialize(50, 20);
		objectWindowLabelScale->Add("Scale", terrainWindow->m_x + 5, shift);
		shift += objectWindowLabelScale->m_height + 1;

		Input* objectScaleX = new Input;
		terrainWindow->addChild(objectScaleX);
		objectScaleX->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectScaleX->setId(27);
		objectScaleX->setText("1.00");
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleX, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleY = new Input;
		terrainWindow->addChild(objectScaleY);
		objectScaleY->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 112, shift);
		objectScaleY->setId(28);
		objectScaleY->setText("1.00");
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleY, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleZ = new Input;
		terrainWindow->addChild(objectScaleZ);
		objectScaleZ->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 214, shift);
		objectScaleZ->setId(29);
		objectScaleZ->setText("1.00");
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleZ, this] {
			this->updateTerrain();
			return 0;
			});
		shift += objectScaleX->m_height + 5;


		Label* objectWindowLabelScaleNormal = new Label;
		terrainWindow->addChild(objectWindowLabelScaleNormal);
		objectWindowLabelScaleNormal->Initialize(50, 20);
		objectWindowLabelScaleNormal->Add("Scale normal", terrainWindow->m_x + 5, shift);
		shift += objectWindowLabelScaleNormal->m_height + 1;

		Input* objectScaleNormalX = new Input;
		terrainWindow->addChild(objectScaleNormalX);
		objectScaleNormalX->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectScaleNormalX->setId(31);
		objectScaleNormalX->setText("1.00");
		objectScaleNormalX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleNormalX, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleNormalY = new Input;
		terrainWindow->addChild(objectScaleNormalY);
		objectScaleNormalY->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 112, shift);
		objectScaleNormalY->setId(32);
		objectScaleNormalY->setText("1.00");
		objectScaleNormalY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleNormalY, this] {
			this->updateTerrain();
			return 0;
			});
		Input* objectScaleNormalZ = new Input;
		terrainWindow->addChild(objectScaleNormalZ);
		objectScaleNormalZ->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 214, shift);
		objectScaleNormalZ->setId(33);
		objectScaleNormalZ->setText("1.00");
		objectScaleNormalZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleNormalZ, this] {
			this->updateTerrain();
			return 0;
			});
		shift += objectScaleNormalX->m_height + 5;


		// layers
		Label* objectWindowLabeLayers = new Label;
		terrainWindow->addChild(objectWindowLabeLayers);
		objectWindowLabeLayers->Initialize(50, 20);
		objectWindowLabeLayers->Add("Layers", terrainWindow->m_x + 5, shift);
		shift += objectWindowLabeLayers->m_height + 1;

		Input* objectLayerAlpha = new Input;
		terrainWindow->addChild(objectLayerAlpha);
		objectLayerAlpha->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectLayerAlpha->setId(40);
		objectLayerAlpha->setText("");
		objectLayerAlpha->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectLayerAlpha, this] {
			this->updateTerrain();
			return 0;
			});
		shift += objectLayerAlpha->m_height + 5;

		Input* objectLayer1 = new Input;
		terrainWindow->addChild(objectLayer1);
		objectLayer1->Initialize(L"data/textures/ui/button.png", 200, 28, terrainWindow->m_x + 10, shift);
		objectLayer1->setId(34);
		objectLayer1->setText("");
		Input* objectLayer1Normal = new Input;
		terrainWindow->addChild(objectLayer1Normal);
		objectLayer1Normal->Initialize(L"data/textures/ui/button.png", 200, 28, terrainWindow->m_x + 212, shift);
		objectLayer1Normal->setId(35);
		objectLayer1Normal->setText("");
		shift += objectLayer1->m_height + 5;

		Input* objectLayer2 = new Input;
		terrainWindow->addChild(objectLayer2);
		objectLayer2->Initialize(L"data/textures/ui/button.png", 200, 28, terrainWindow->m_x + 10, shift);
		objectLayer2->setId(36);
		objectLayer2->setText("");
		Input* objectLayer2Normal = new Input;
		terrainWindow->addChild(objectLayer2Normal);
		objectLayer2Normal->Initialize(L"data/textures/ui/button.png", 200, 28, terrainWindow->m_x + 212, shift);
		objectLayer2Normal->setId(37);
		objectLayer2Normal->setText("");
		shift += objectLayer2->m_height + 5;

		Input* objectLayer3 = new Input;
		terrainWindow->addChild(objectLayer3);
		objectLayer3->Initialize(L"data/textures/ui/button.png", 200, 28, terrainWindow->m_x + 10, shift);
		objectLayer3->setId(38);
		objectLayer3->setText("");
		Input* objectLayer3Normal = new Input;
		terrainWindow->addChild(objectLayer3Normal);
		objectLayer3Normal->Initialize(L"data/textures/ui/button.png", 200, 28, terrainWindow->m_x + 212, shift);
		objectLayer3Normal->setId(39);
		objectLayer3Normal->setText("");
		shift += objectLayer3->m_height + 5;
	}

	void InitWindowCompositeObject()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_uiManager->getById(1));

		Window* objectWindow = new Window;
		m_uiManager->Add(objectWindow);
		objectWindow->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		objectWindow->addHeader("Composite Object properties", Window::HEADER_BUTTON_CLOSE);
		objectWindow->addBody();
		objectWindow->hide();
		objectWindow->setId(41);
		objectWindow->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			unselectModel();
			return 0;
			});
		shift += objectWindow->m_y + objectWindow->getHeader()->m_height + 5;


		Input* object1Id = new Input;
		objectWindow->addChild(object1Id);
		object1Id->Initialize(L"data/textures/ui/button.png", 400, 28, objectWindow->m_x + 10, shift);
		object1Id->setText("");
		object1Id->setId(42);
		object1Id->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [object1Id, this] {
			int id = std::stoi(object1Id->getValue());
			AbstractModel* child = m_modelManager->getById(id);
			if (child) {
				MapEntity::ObjectFormat* childFormat = getObjectEditor(id);
				if (!childFormat->parent) {
					CompositeModel* model = new CompositeModel;
					model->setId(m_modelManager->getNextId());
					model->addChild(child);
					m_modelManager->Add(model);
					childFormat->parent = model;
					m_selectedModel = model;

					MapEntity::ObjectFormat format;
					format.id = model->getId();
					format.type = MapEntity::ObjectTypes::COMPOSITE_MODEL;
					format.position = model->GetPosition();
					format.scale = model->GetScale();
					format.rotation = model->getRotation();
					format.extraParams.push_back(std::to_string(id));
					format.extraParams.push_back("");
					m_mapEntities->add(format);
				}
				else {
					m_selectedModel = childFormat->parent;
				}
				this->updateCompositeModel();
			}
			return 0;
			});
		shift += object1Id->m_height + 5;

		Input* object2Id = new Input;
		objectWindow->addChild(object2Id);
		object2Id->Initialize(L"data/textures/ui/button.png", 400, 28, objectWindow->m_x + 10, shift);
		object2Id->setText("");
		object2Id->setId(43);
		object2Id->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [object1Id, object2Id, this] {
			int id = std::stoi(object1Id->getValue());
			int id2 = std::stoi(object2Id->getValue());
			AbstractModel* child = m_modelManager->getById(id);
			AbstractModel* child2 = m_modelManager->getById(id2);
			if (child) {
				MapEntity::ObjectFormat* childFormat = getObjectEditor(id);
				MapEntity::ObjectFormat* childFormat2 = getObjectEditor(id2);
				if (childFormat->parent) {
					CompositeModel* model = childFormat->parent;
					model->addChild(child2);
					childFormat2->parent = childFormat->parent;
					m_selectedModel = childFormat->parent;
				}
				this->updateCompositeModel();
			}
			return 0;
			});
		shift += object2Id->m_height + 5;



		Label* objectWindowLabelPosition = new Label;
		objectWindow->addChild(objectWindowLabelPosition);
		objectWindowLabelPosition->Initialize(50, 20);
		objectWindowLabelPosition->Add("Position", objectWindow->m_x + 5, shift);
		shift += objectWindowLabelPosition->m_height + 1;

		Input* objectPositionX = new Input;
		objectWindow->addChild(objectPositionX);
		objectPositionX->Initialize(L"data/textures/ui/button.png", 100, 28, objectWindow->m_x + 10, shift);
		objectPositionX->setId(44);
		objectPositionX->setText("0.00");
		objectPositionX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionX, this] {
			this->updateCompositeModel();
			return 0;
			});
		Input* objectPositionY = new Input;
		objectWindow->addChild(objectPositionY);
		objectPositionY->Initialize(L"data/textures/ui/button.png", 100, 28, objectWindow->m_x + 112, shift);
		objectPositionY->setId(45);
		objectPositionY->setText("0.00");
		objectPositionY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionY, this] {
			this->updateCompositeModel();
			return 0;
			});
		Input* objectPositionZ = new Input;
		objectWindow->addChild(objectPositionZ);
		objectPositionZ->Initialize(L"data/textures/ui/button.png", 100, 28, objectWindow->m_x + 214, shift);
		objectPositionZ->setId(46);
		objectPositionZ->setText("0.00");
		objectPositionZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionZ, this] {
			this->updateCompositeModel();
			return 0;
			});
		shift += objectPositionX->m_height + 5;


		Label* objectWindowLabelScale = new Label;
		objectWindow->addChild(objectWindowLabelScale);
		objectWindowLabelScale->Initialize(50, 20);
		objectWindowLabelScale->Add("Scale", objectWindow->m_x + 5, shift);
		shift += objectWindowLabelScale->m_height + 1;

		Input* objectScaleX = new Input;
		objectWindow->addChild(objectScaleX);
		objectScaleX->Initialize(L"data/textures/ui/button.png", 100, 28, objectWindow->m_x + 10, shift);
		objectScaleX->setId(47);
		objectScaleX->setText("1.00");
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleX, this] {
			this->updateCompositeModel();
			return 0;
			});
		Input* objectScaleY = new Input;
		objectWindow->addChild(objectScaleY);
		objectScaleY->Initialize(L"data/textures/ui/button.png", 100, 28, objectWindow->m_x + 112, shift);
		objectScaleY->setId(48);
		objectScaleY->setText("1.00");
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleY, this] {
			this->updateCompositeModel();
			return 0;
			});
		Input* objectScaleZ = new Input;
		objectWindow->addChild(objectScaleZ);
		objectScaleZ->Initialize(L"data/textures/ui/button.png", 100, 28, objectWindow->m_x + 214, shift);
		objectScaleZ->setId(49);
		objectScaleZ->setText("1.00");
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleZ, this] {
			this->updateCompositeModel();
			return 0;
			});
		shift += objectScaleX->m_height + 5;
	}


	void InitWindowWater()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_uiManager->getById(1));

		Window* terrainWindow = new Window;
		m_uiManager->Add(terrainWindow);
		terrainWindow->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		terrainWindow->addHeader("Water Node properties", Window::HEADER_BUTTON_CLOSE);
		terrainWindow->addBody();
		terrainWindow->hide();
		terrainWindow->setId(51);
		terrainWindow->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			unselectModel();
			return 0;
			});
		shift += terrainWindow->m_y + terrainWindow->getHeader()->m_height + 5;


		Input* objectPath = new Input;
		terrainWindow->addChild(objectPath);
		objectPath->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectPath->setText("data/models/water.ds");
		objectPath->setId(52);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPath, this] {
			if (!m_selectedModel) {
				Input* objectTexture = dynamic_cast<Input*>(m_uiManager->getById(53));

				WaterNode* model = new WaterNode;
				bool result = model->Initialize(m_Graphics->getD3D(), &objectPath->getValue()[0], { objectTexture->getValue() });
				if (result) {
					model->setId(m_modelManager->getNextId());
					model->addLights({ m_light });

					m_modelManager->Add(model);
					m_selectedModel = model;

					AbstractGui* objectWindow = m_uiManager->getById(51);
					objectWindow->show();
					objectWindow->focus();

					MapEntity::ObjectFormat format;
					format.id = model->getId();
					format.type = MapEntity::ObjectTypes::WATER;
					format.position = model->GetPosition();
					format.scale = model->GetScale();
					format.rotation = model->getRotation();
					format.path = objectPath->getValue();
					format.texture = objectTexture->getValue();
					// water height
					format.extraParams.push_back("");
					// tiling
					format.extraParams.push_back("");
					format.extraParams.push_back("");
					// refreaction scale
					format.extraParams.push_back("");
					// target
					format.extraParams.push_back("");

					m_mapEntities->add(format);

					this->updateWater();
				}
			}
			else {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->path = objectPath->getValue();
				this->updateWater();
			}
			return 0;
			});
		shift += objectPath->m_height + 5;


		Input* objectTexture = new Input;
		terrainWindow->addChild(objectTexture);
		objectTexture->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectTexture->setText("data/textures/water01.dds");
		objectTexture->setId(53);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->texture = objectTexture->getValue();
				this->updateWater();
			}
			return 0;
			});
		shift += objectTexture->m_height + 5;

		Label* objectWindowLabelPosition = new Label;
		terrainWindow->addChild(objectWindowLabelPosition);
		objectWindowLabelPosition->Initialize(50, 20);
		objectWindowLabelPosition->Add("Position", terrainWindow->m_x + 5, shift);
		shift += objectWindowLabelPosition->m_height + 1;

		Input* objectPositionX = new Input;
		terrainWindow->addChild(objectPositionX);
		objectPositionX->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectPositionX->setId(54);
		objectPositionX->setText("0.00");
		objectPositionX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionX, this] {
			this->updateWater();
			return 0;
			});
		Input* objectPositionY = new Input;
		terrainWindow->addChild(objectPositionY);
		objectPositionY->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 112, shift);
		objectPositionY->setId(55);
		objectPositionY->setText("0.00");
		objectPositionY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionY, this] {
			this->updateWater();
			return 0;
			});
		Input* objectPositionZ = new Input;
		terrainWindow->addChild(objectPositionZ);
		objectPositionZ->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 214, shift);
		objectPositionZ->setId(56);
		objectPositionZ->setText("0.00");
		objectPositionZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPositionZ, this] {
			this->updateWater();
			return 0;
			});
		shift += objectPositionX->m_height + 5;


		Label* objectWindowLabelScale = new Label;
		terrainWindow->addChild(objectWindowLabelScale);
		objectWindowLabelScale->Initialize(50, 20);
		objectWindowLabelScale->Add("Scale", terrainWindow->m_x + 5, shift);
		shift += objectWindowLabelScale->m_height + 1;

		Input* objectScaleX = new Input;
		terrainWindow->addChild(objectScaleX);
		objectScaleX->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectScaleX->setId(57);
		objectScaleX->setText("1.00");
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleX, this] {
			this->updateWater();
			return 0;
			});
		Input* objectScaleY = new Input;
		terrainWindow->addChild(objectScaleY);
		objectScaleY->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 112, shift);
		objectScaleY->setId(58);
		objectScaleY->setText("1.00");
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleY, this] {
			this->updateWater();
			return 0;
			});
		Input* objectScaleZ = new Input;
		terrainWindow->addChild(objectScaleZ);
		objectScaleZ->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 214, shift);
		objectScaleZ->setId(59);
		objectScaleZ->setText("1.00");
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleZ, this] {
			this->updateWater();
			return 0;
			});
		shift += objectScaleX->m_height + 5;


		Input* objectWaterHeight = new Input;
		terrainWindow->addChild(objectWaterHeight);
		objectWaterHeight->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectWaterHeight->setText("5");
		objectWaterHeight->setId(60);
		objectWaterHeight->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterHeight, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->extraParams[0] = objectWaterHeight->getValue();
				this->updateWater();
			}
			return 0;
			});
		shift += objectWaterHeight->m_height + 5;

		Input* objectWaterTilingX = new Input;
		terrainWindow->addChild(objectWaterTilingX);
		objectWaterTilingX->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectWaterTilingX->setText("0.05");
		objectWaterTilingX->setId(61);
		objectWaterTilingX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterTilingX, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->extraParams[1] = objectWaterTilingX->getValue();
				this->updateWater();
			}
			return 0;
			});
		Input* objectWaterTilingY = new Input;
		terrainWindow->addChild(objectWaterTilingY);
		objectWaterTilingY->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + objectWaterTilingX->m_width + 20, shift);
		objectWaterTilingY->setText("0.05");
		objectWaterTilingY->setId(62);
		objectWaterTilingY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterTilingY, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->extraParams[2] = objectWaterTilingY->getValue();
				this->updateWater();
			}
			return 0;
			});
		shift += objectWaterTilingX->m_height + 5;

		Input* objectWaterRefractionScale = new Input;
		terrainWindow->addChild(objectWaterRefractionScale);
		objectWaterRefractionScale->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectWaterRefractionScale->setText("0.02");
		objectWaterRefractionScale->setId(63);
		objectWaterRefractionScale->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterRefractionScale, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->extraParams[3] = objectWaterRefractionScale->getValue();
				this->updateWater();
			}
			return 0;
			});
		shift += objectWaterRefractionScale->m_height + 5;


		Input* objectWaterTarget = new Input;
		terrainWindow->addChild(objectWaterTarget);
		objectWaterTarget->Initialize(L"data/textures/ui/button.png", 100, 28, terrainWindow->m_x + 10, shift);
		objectWaterTarget->setText("");
		objectWaterTarget->setId(64);
		objectWaterTarget->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterTarget, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->extraParams[4] = objectWaterTarget->getValue();
				this->updateWater();
			}
			return 0;
			});
		shift += objectWaterTarget->m_height + 5;
	}

	void InitWindowSky()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_uiManager->getById(1));

		Window* terrainWindow = new Window;
		m_uiManager->Add(terrainWindow);
		terrainWindow->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		terrainWindow->addHeader("Sky Node properties", Window::HEADER_BUTTON_CLOSE);
		terrainWindow->addBody();
		terrainWindow->hide();
		terrainWindow->setId(71);
		terrainWindow->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			unselectModel();
			return 0;
			});
		shift += terrainWindow->m_y + terrainWindow->getHeader()->m_height + 5;


		Input* objectPath = new Input;
		terrainWindow->addChild(objectPath);
		objectPath->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectPath->setText("data/models/skydome.ds");
		objectPath->setId(72);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPath, this] {
			if (!m_selectedModel) {
				Input* objectTexture1 = dynamic_cast<Input*>(m_uiManager->getById(73));
				Input* objectTexture2 = dynamic_cast<Input*>(m_uiManager->getById(74));

				SkyPlaneClass* model = new SkyPlaneClass;
				SkyDomeClass* modelDome = new SkyDomeClass;
				bool result = model->Initialize(m_Graphics->getD3D(), { &objectTexture1->getValue()[0], &objectTexture2->getValue()[0] });
				if (result) {
					modelDome->Initialize(m_Graphics->getD3D(), objectPath->getValue());

					modelDome->setId(m_modelManager->getNextId());
					m_modelManager->Add(modelDome);

					model->setId(m_modelManager->getNextId());
					m_modelManager->Add(model);
					m_selectedModel = model;

					AbstractGui* objectWindow = m_uiManager->getById(71);
					objectWindow->show();
					objectWindow->focus();

					MapEntity::ObjectFormat format;
					format.id = model->getId();
					format.type = MapEntity::ObjectTypes::SKY;
					format.path = objectPath->getValue();
					format.position = model->GetPosition();
					format.scale = model->GetScale();
					format.rotation = model->getRotation();
					format.texture = objectTexture1->getValue();
					// perturbe
					format.extraParams.push_back(objectTexture2->getValue());
					// dome
					format.extraParams.push_back(std::to_string(modelDome->getId()));

					m_mapEntities->add(format);
				}
			}
			else {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->path = objectPath->getValue();
				this->updateSky();
			}
			return 0;
			});
		shift += objectPath->m_height + 5;


		Input* objectTexture = new Input;
		terrainWindow->addChild(objectTexture);
		objectTexture->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectTexture->setText("data/textures/cloud001.dds");
		objectTexture->setId(73);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->texture = objectTexture->getValue();
				this->updateSky();
			}
			return 0;
			});
		shift += objectTexture->m_height + 5;

		Input* objectTexture2 = new Input;
		terrainWindow->addChild(objectTexture2);
		objectTexture2->Initialize(L"data/textures/ui/button.png", 400, 28, terrainWindow->m_x + 10, shift);
		objectTexture2->setText("data/textures/perturb001.dds");
		objectTexture2->setId(74);
		objectTexture2->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture2, this] {
			if (m_selectedModel) {
				MapEntity::ObjectFormat* editorFormat = this->getObjectEditor(m_selectedModel->getId());
				editorFormat->extraParams[0] = objectTexture2->getValue();
				this->updateSky();
			}
			return 0;
			});
		shift += objectTexture2->m_height + 5;
	}*/

bool App::init()
{
	if (!SystemClass::init()) {
		return false;
	}

	int size = sizeof(m_Counters) / sizeof(m_Counters[0]);
	for (int i = 0; i < size; i++) {
		m_Counters[i] = 0;
	}

	m_Position->SetPosition(8.0f, 10.0f, 30.0f);
	m_Position->SetRotation(15.0f, -180.0f, 0.0f);
	m_modelManager = m_Graphics->getModelManager();
	m_uiManager = m_Graphics->getUiManager();

	m_selectedModel = 0;
	m_CollisionDetection = new CollisionDetection;
	m_mapEntities = new MapEntity;

	loadUI();
	loadScene();

	return true;
}

void App::loadUI()
{
	int shift;

	Label* label = new Label;
	m_uiManager->add(label);
	label->Initialize(76, 28);
	label->Add("Hello World", 1, 60);
	label->setId(4);

	Label* label2 = new Label;
	m_uiManager->add(label2);
	label2->Initialize(76, 28);
	label2->Add("", 1, 80, 1.0f, 0.3f, 0.3f);
	label2->setId(5);


	this->IninMenuTop();
	/*this->InitWindowObject();
	this->InitWindowCompositeObject();
	this->InitWindowTerrain();
	this->InitWindowWater();
	this->InitWindowSky();*/


	Cursor* cursor = new Cursor;
	m_uiManager->add(cursor);
	cursor->Initialize(32, 32);
	cursor->setId(6);
}

void App::loadScene()
{
	m_selectedModel = 0;

	m_light = new LightClass;
	m_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -1.0f, 1.0f);
	m_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetSpecularPower(32.0f);

	initDefaultObjects();
}

bool App::frame()
{
	if (!SystemClass::frame()) {
		return false;
	}

	frameUI();
	frameScene();

	return true;
}


void App::frameUI()
{
	int mouseX, mouseY;
	m_Input->GetMouseLocation(mouseX, mouseY);
	int mouseButtonPress = m_Input->getMouseButtonPress();
	D3DXVECTOR3 direction, position, scale, rotation;
	int size;

	if (m_Input->IsKeyDown(VK_ESCAPE)) {
		m_uiManager->unfocus();
	}

	if (mouseButtonPress == MOUSE_BUTTON1 && !m_uiManager->isFocused()) {
		m_CollisionDetection->m_position = m_Graphics->getCamera()->GetPosition();
		m_Graphics->getD3D()->GetProjectionMatrix(m_CollisionDetection->m_projectionMatrix);
		m_Graphics->getD3D()->GetWorldMatrix(m_CollisionDetection->m_worldMatrix);
		m_Graphics->getCamera()->GetViewMatrix(m_CollisionDetection->m_viewMatrix);

		std::vector<AbstractModel*> models = m_modelManager->getModels();
		std::vector<AbstractModel*> modelsSelected;

		modelsSelected.clear();
		this->unselectModel();

		for (int i = 0; i < models.size(); i++) {
			if (models[i]->getId() <= 1) {
				continue;
			}
			ModelClass* model = dynamic_cast<ModelClass*>(models[i]);

			m_CollisionDetection->getRayDirection(m_Graphics->getD3D()->getScreenWidth(), m_Graphics->getD3D()->getScreenHeight(), mouseX, mouseY, position, direction);
			if (m_CollisionDetection->RayBoxIntersect(position, direction, model->getMinPosition(), model->getMaxPosition())) {
				modelsSelected.push_back(model);
			}
		}

		Model* modelNear = 0;
		float minDistance = D3D11_FLOAT32_MAX;
		float distance;
		for (int i = 0; i < modelsSelected.size(); i++) {
			Model* model = dynamic_cast<Model*>(modelsSelected[i]);
			distance = m_CollisionDetection->getDistanceByPoints(m_CollisionDetection->m_position, model->getCenter());
			if (distance < minDistance) {
				minDistance = distance;
				modelNear = model;
			}
		}

		if (modelNear) {
			m_selectedModel = modelNear;
			m_selectedModel->showBBox();
			MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
			if (editorFormat->type == MapEntity::ObjectTypes::MODEL) {
				m_objectUI->updateUiFromModel();
			}
			/*else if (editorFormat->type == MapEntity::ObjectTypes::COMPOSITE_MODEL) {
				this->updateCompositeModel();
			}
			else if (editorFormat->type == MapEntity::ObjectTypes::TERRAIN) {
				this->updateWindowTerrain();
			}
			else if (editorFormat->type == MapEntity::ObjectTypes::WATER) {
				this->updateWindowWater();
			}
			else if (editorFormat->type == MapEntity::ObjectTypes::SKY) {
				this->updateWindowSky();
			}*/
		}
	}

	std::vector<AbstractGui*> elements = m_uiManager->getElements();
	size = elements.size();
	for (int i = 0; i < size; i++) {
		if (elements[i]->getId() == 4) {
			Label* label = dynamic_cast<Label*>(elements[i]);
			char mouseString[128];
			sprintf(mouseString, "Fps: %d, Cpu: %3.2f%%, MouseX: %d, MouseY: %d, MouseButton: %d", m_Fps->GetFps(), m_Fps->GetCpuPercentage(), mouseX, mouseY, mouseButtonPress);
			label->Add(mouseString, 10, 50);
		}
		if (elements[i]->getId() == 5) {
			Label* label = dynamic_cast<Label*>(elements[i]);
			char string[128];
			sprintf(string, "Coord: %.2f:%.2f:%.2f, Render objects: %d, triangles: %d", m_Graphics->getCamera()->GetPosition().x, m_Graphics->getCamera()->GetPosition().y, m_Graphics->getCamera()->GetPosition().z, m_modelManager->getRenderCount(), m_modelManager->getTriangleCount());
			label->Add(string, 10, 70, 1.0f, 1.0f, 0.5f);
		}
		if (elements[i]->getId() == 6) {
			Cursor* cursor = dynamic_cast<Cursor*>(elements[i]);
			if (mouseButtonPress == MOUSE_BUTTON2 || m_uiManager->isFocused()) {
				cursor->setPosition(mouseX, mouseY);
				cursor->show();
			}
			else {
				cursor->hide();
			}
		}
	}
}

void App::frameScene()
{
	float time = m_Timer->GetTime();
}

MapEntity::ObjectFormat* App::getObjectEditor(int id)
{
	return m_mapEntities->getEntity(id);
}

void App::unselectModel()
{
	if (m_selectedModel) {
		m_selectedModel->hideBBox();
		m_selectedModel = 0;

		/*AbstractGui* objectWindow = m_uiManager->getById(2);
		objectWindow->hide();

		AbstractGui* terrainWindow = m_uiManager->getById(21);
		terrainWindow->hide();*/
	}
}

	/*void resetWindowModel()
	{
		Input* positionInput;
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(10));
		positionInput->setText(std::string("0.00"));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(11));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(12));
		positionInput->setText("0.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(13));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(14));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(15));
		positionInput->setText("1.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(16));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(17));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(18));
		positionInput->setText("0.00");
	}

	void updateWindowModel()
	{
		D3DXVECTOR3 direction, position, scale, rotation;
		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());

		this->updateBbox();

		position = m_selectedModel->GetPosition();
		scale = m_selectedModel->GetScale();
		rotation = m_selectedModel->getRotation();

		Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(2));
		objectWindow->show();
		objectWindow->focus();
		std::string title = "Object properties - ";
		title += std::to_string(m_selectedModel->getId());
		objectWindow->setTitle(title);
		Input* positionInput;
		Checkbox* checkbox;

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(10));
		positionInput->setText(String::ssprintf("%.2f", position.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(11));
		positionInput->setText(String::ssprintf("%.2f", position.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(12));
		positionInput->setText(String::ssprintf("%.2f", position.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(13));
		positionInput->setText(String::ssprintf("%.2f", scale.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(14));
		positionInput->setText(String::ssprintf("%.2f", scale.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(15));
		positionInput->setText(String::ssprintf("%.2f", scale.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(16));
		positionInput->setText(String::ssprintf("%.2f", rotation.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(17));
		positionInput->setText(String::ssprintf("%.2f", rotation.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(18));
		positionInput->setText(String::ssprintf("%.2f", rotation.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(19));
		positionInput->setText(editorFormat->path);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(20));
		positionInput->setText(editorFormat->texture);

		if (editorFormat->extraParams.size() < 1) {
			editorFormat->extraParams.push_back("0");
		}
		checkbox = dynamic_cast<Checkbox*>(m_uiManager->getById(75));
		checkbox->setMarked(stoi(editorFormat->extraParams[0]) > 0);
	}

	void updateObjectModel()
	{
		if (!m_selectedModel) {
			return;
		}

		Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(2));
		std::string title = "Object properties - ";
		title += std::to_string(m_selectedModel->getId());
		objectWindow->setTitle(title);

		Input* input;
		Checkbox* checkbox;
		D3DXVECTOR3 position, scale, rotation;
		bool isAlpha;
		std::string path, texture;

		input = dynamic_cast<Input*>(m_uiManager->getById(10));
		position.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(11));
		position.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(12));
		position.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(13));
		scale.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(14));
		scale.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(15));
		scale.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(16));
		rotation.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(17));
		rotation.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(18));
		rotation.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(19));
		path = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(20));
		texture = input->getValue();

		checkbox = dynamic_cast<Checkbox*>(m_uiManager->getById(75));
		isAlpha = checkbox->getIsMarked();


		m_selectedModel->setAlpha(isAlpha);

		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		editorFormat->position = position;
		editorFormat->scale = scale;
		editorFormat->rotation = rotation;
		editorFormat->path = path;
		editorFormat->texture = texture;
		editorFormat->extraParams[0] = std::to_string(isAlpha ? 1 : 0);

		this->updateBbox();
	}

	void resetWindowCompositeModel()
	{
		Input* positionInput;

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(42));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(43));
		positionInput->setText("");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(44));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(45));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(46));
		positionInput->setText("0.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(47));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(48));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(49));
		positionInput->setText("1.00");
	}

	void updateWindowCompositeModel()
	{
		D3DXVECTOR3 direction, position, scale, rotation, scaleNormal;
		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		CompositeModel* model = dynamic_cast<CompositeModel*>(m_selectedModel);

		this->updateBbox();

		position = editorFormat->position;
		scale = editorFormat->scale;

		AbstractGui* objectWindow = m_uiManager->getById(41);
		objectWindow->show();
		objectWindow->focus();
		Input* positionInput;

		if (editorFormat->extraParams.size() > 0) {
			positionInput = dynamic_cast<Input*>(m_uiManager->getById(42));
			positionInput->setText(String::ssprintf("%s", editorFormat->extraParams[0]));
			if (editorFormat->extraParams.size() > 1) {
				positionInput = dynamic_cast<Input*>(m_uiManager->getById(43));
				positionInput->setText(String::ssprintf("%s", editorFormat->extraParams[1]));
			}
		}

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(44));
		positionInput->setText(String::ssprintf("%.2f", position.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(45));
		positionInput->setText(String::ssprintf("%.2f", position.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(46));
		positionInput->setText(String::ssprintf("%.2f", position.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(47));
		positionInput->setText(String::ssprintf("%.2f", scale.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(48));
		positionInput->setText(String::ssprintf("%.2f", scale.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(49));
		positionInput->setText(String::ssprintf("%.2f", scale.z));
	}

	void updateCompositeModel()
	{
		if (!m_selectedModel) {
			return;
		}

		Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(41));
		std::string title = "Composite object properties - ";
		title += std::to_string(m_selectedModel->getId());
		objectWindow->setTitle(title);
		Input* input;

		D3DXVECTOR3 position, scale;
		std::string childId1, childId2;

		input = dynamic_cast<Input*>(m_uiManager->getById(42));
		childId1 = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(43));
		childId2 = input->getValue();

		input = dynamic_cast<Input*>(m_uiManager->getById(44));
		position.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(45));
		position.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(46));
		position.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(47));
		scale.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(48));
		scale.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(49));
		scale.z = std::stof(input->getValue());


		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		editorFormat->position = position;
		editorFormat->scale = scale;
		editorFormat->extraParams[0] = childId1;
		editorFormat->extraParams[1] = childId2;

		CompositeModel* model = dynamic_cast<CompositeModel*>(m_selectedModel);

		model->SetPosition(position);
		model->SetScale(scale);

		if (childId1.size() > 0) {
			model->removeChilds();
			model->addChild(m_modelManager->getById(stoi(childId1)));
			if (childId2.size() > 0) {
				model->addChild(m_modelManager->getById(stoi(childId2)));
			}
		}

		this->updateBbox();
	}

	void resetWindowTerrain()
	{
		Input* positionInput;
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(24));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(25));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(26));
		positionInput->setText("0.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(27));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(28));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(29));
		positionInput->setText("1.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(31));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(32));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(33));
		positionInput->setText("1.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(40));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(34));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(35));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(36));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(37));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(38));
		positionInput->setText("");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(39));
		positionInput->setText("");
	}

	void updateWindowTerrain()
	{
		D3DXVECTOR3 direction, position, scale, rotation, scaleNormal;
		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		TerrainClass* terrain = dynamic_cast<TerrainClass*>(m_selectedModel);

		this->updateBbox();

		position = editorFormat->position;
		scale = editorFormat->scale;
		scaleNormal = terrain->getScaleNormal();

		AbstractGui* objectWindow = m_uiManager->getById(21);
		objectWindow->show();
		objectWindow->focus();
		Input* positionInput;

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(24));
		positionInput->setText(String::ssprintf("%.2f", position.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(25));
		positionInput->setText(String::ssprintf("%.2f", position.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(26));
		positionInput->setText(String::ssprintf("%.2f", position.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(27));
		positionInput->setText(String::ssprintf("%.2f", scale.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(28));
		positionInput->setText(String::ssprintf("%.2f", scale.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(29));
		positionInput->setText(String::ssprintf("%.2f", scale.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(31));
		positionInput->setText(String::ssprintf("%.2f", scaleNormal.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(32));
		positionInput->setText(String::ssprintf("%.2f", scaleNormal.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(33));
		positionInput->setText(String::ssprintf("%.2f", scaleNormal.z));

		// layers
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(40));
		positionInput->setText(editorFormat->extraParams[2]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(34));
		positionInput->setText(editorFormat->extraParams[3]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(35));
		positionInput->setText(editorFormat->extraParams[4]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(36));
		positionInput->setText(editorFormat->extraParams[5]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(37));
		positionInput->setText(editorFormat->extraParams[6]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(38));
		positionInput->setText(editorFormat->extraParams[7]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(39));
		positionInput->setText(editorFormat->extraParams[8]);

		// textures
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(22));
		positionInput->setText(editorFormat->path);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(23));
		positionInput->setText(editorFormat->texture);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(30));
		positionInput->setText(editorFormat->extraParams[0]);
	}

	void updateTerrain()
	{
		if (!m_selectedModel) {
			return;
		}

		Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(21));
		std::string title = "Terrain properties - ";
		title += std::to_string(m_selectedModel->getId());
		objectWindow->setTitle(title);

		Input* input;

		D3DXVECTOR3 position, scale, scaleNormal;
		std::string textures, texturesNormal, heightmap;
		std::string layerAlpha, layer1, layer1Normal, layer2, layer2Normal, layer3, layer3Normal;

		input = dynamic_cast<Input*>(m_uiManager->getById(24));
		position.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(25));
		position.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(26));
		position.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(27));
		scale.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(28));
		scale.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(29));
		scale.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(31));
		scaleNormal.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(32));
		scaleNormal.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(33));
		scaleNormal.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(22));
		heightmap = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(23));
		textures = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(30));
		texturesNormal = input->getValue();

		input = dynamic_cast<Input*>(m_uiManager->getById(40));
		layerAlpha = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(34));
		layer1 = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(35));
		layer1Normal = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(36));
		layer2 = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(37));
		layer2Normal = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(38));
		layer3 = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(39));
		layer3Normal = input->getValue();


		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		editorFormat->position = position;
		editorFormat->scale = scale;
		editorFormat->path = heightmap;
		editorFormat->texture = textures;
		editorFormat->extraParams[0] = texturesNormal;
		editorFormat->extraParams[1] = std::to_string(scaleNormal.x)+";"+ std::to_string(scaleNormal.y)+";"+ std::to_string(scaleNormal.z);
		editorFormat->extraParams[2] = layerAlpha;
		editorFormat->extraParams[3] = layer1;
		editorFormat->extraParams[4] = layer1Normal;
		editorFormat->extraParams[5] = layer2;
		editorFormat->extraParams[6] = layer2Normal;
		editorFormat->extraParams[7] = layer3;
		editorFormat->extraParams[8] = layer3Normal;


		TerrainClass* terrain = dynamic_cast<TerrainClass*>(m_selectedModel);

		terrain->Shutdown();
		terrain->SetPosition(position);
		terrain->SetScale(scale);
		terrain->setScaleNormal(scaleNormal);
		terrain->Initialize(m_Graphics->getD3D(), m_Graphics->getFrustum(), &heightmap[0], textures, texturesNormal);
		if (layerAlpha.length() > 1 && layer1.length() > 1 && layer1Normal.length() > 1) {
			terrain->addTextureLayer(layer1, layer1Normal);
			if (layer2.length() > 1 && layer2Normal.length() > 1) {
				terrain->addTextureLayer(layer2, layer2Normal);
				if (layer3.length() > 1 && layer3Normal.length() > 1) {
					terrain->addTextureLayer(layer3, layer3Normal);
				}
			}
			terrain->addTextureAlpha(layerAlpha);
		}

		this->updateBbox();
	}


	void resetWindowWater()
	{
		Input* positionInput;
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(54));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(55));
		positionInput->setText("0.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(56));
		positionInput->setText("0.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(57));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(58));
		positionInput->setText("1.00");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(59));
		positionInput->setText("1.00");

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(60));
		positionInput->setText("5");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(61));
		positionInput->setText("0.05");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(62));
		positionInput->setText("0.05");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(63));
		positionInput->setText("0.02");
	}

	void updateWindowWater()
	{
		D3DXVECTOR3 position, scale;
		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		WaterNode* model = dynamic_cast<WaterNode*>(m_selectedModel);

		this->updateBbox();

		position = editorFormat->position;
		scale = editorFormat->scale;

		AbstractGui* objectWindow = m_uiManager->getById(51);
		objectWindow->show();
		objectWindow->focus();
		Input* positionInput;

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(52));
		positionInput->setText(editorFormat->path);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(53));
		positionInput->setText(editorFormat->texture);

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(54));
		positionInput->setText(String::ssprintf("%.2f", position.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(55));
		positionInput->setText(String::ssprintf("%.2f", position.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(56));
		positionInput->setText(String::ssprintf("%.2f", position.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(57));
		positionInput->setText(String::ssprintf("%.2f", scale.x));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(58));
		positionInput->setText(String::ssprintf("%.2f", scale.y));
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(59));
		positionInput->setText(String::ssprintf("%.2f", scale.z));

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(60));
		positionInput->setText(editorFormat->extraParams[0]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(61));
		positionInput->setText(editorFormat->extraParams[1]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(62));
		positionInput->setText(editorFormat->extraParams[2]);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(63));
		positionInput->setText(editorFormat->extraParams[3]);
	}

	void updateWater()
	{
		if (!m_selectedModel) {
			return;
		}

		Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(51));
		std::string title = "Water properties - ";
		title += std::to_string(m_selectedModel->getId());
		objectWindow->setTitle(title);

		Input* input;

		D3DXVECTOR3 position, scale, scaleNormal;
		std::string path, textures;
		std::string waterHeight, waterTilingX, waterTilingY, waterRefraction, waterTarget;

		input = dynamic_cast<Input*>(m_uiManager->getById(52));
		path = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(53));
		textures = input->getValue();

		input = dynamic_cast<Input*>(m_uiManager->getById(54));
		position.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(55));
		position.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(56));
		position.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(57));
		scale.x = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(58));
		scale.y = std::stof(input->getValue());
		input = dynamic_cast<Input*>(m_uiManager->getById(59));
		scale.z = std::stof(input->getValue());

		input = dynamic_cast<Input*>(m_uiManager->getById(60));
		waterHeight = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(61));
		waterTilingX = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(62));
		waterTilingY = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(63));
		waterRefraction = input->getValue();

		input = dynamic_cast<Input*>(m_uiManager->getById(64));
		waterTarget = input->getValue();


		ModelClass* modelWaterTarget = 0;
		if (waterTarget.size() > 0) {
			modelWaterTarget = dynamic_cast<ModelClass*>(m_modelManager->getById(std::stoi(waterTarget)));
		}
		if (!modelWaterTarget) {
			waterTarget = "";
			modelWaterTarget = 0;
		}


		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		editorFormat->position = position;
		editorFormat->scale = scale;
		editorFormat->path = path;
		editorFormat->texture = textures;
		editorFormat->extraParams[0] = waterHeight;
		editorFormat->extraParams[1] = waterTilingX;
		editorFormat->extraParams[2] = waterTilingY;
		editorFormat->extraParams[3] = waterRefraction;
		editorFormat->extraParams[4] = waterTarget;

		WaterNode* model = dynamic_cast<WaterNode*>(m_selectedModel);

		model->Shutdown();
		model->Initialize(m_Graphics->getD3D(), &path[0], { textures });
		model->SetPosition(position);
		model->SetScale(scale);
		model->setWaterHeight(std::stof(waterHeight));
		model->setNormalMapTiling(std::stof(waterTilingX), std::stof(waterTilingY));
		model->setReflectRefractScale(std::stof(waterRefraction));
		if (modelWaterTarget) {
			model->addRefractionTarget(modelWaterTarget);
		}

		this->updateBbox();
	}

	void resetWindowSky()
	{
		Input* positionInput;
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(72));
		positionInput->setText("data/models/skydome.ds");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(73));
		positionInput->setText("data/textures/cloud001.dds");
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(74));
		positionInput->setText("data/textures/perturb001.dds");
	}

	void updateWindowSky()
	{
		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		SkyPlaneClass* model = dynamic_cast<SkyPlaneClass*>(m_selectedModel);
		SkyDomeClass* modelDome = dynamic_cast<SkyDomeClass*>(m_modelManager->getById(stoi(editorFormat->extraParams[1])));

		AbstractGui* objectWindow = m_uiManager->getById(71);
		objectWindow->show();
		objectWindow->focus();
		Input* positionInput;

		positionInput = dynamic_cast<Input*>(m_uiManager->getById(72));
		positionInput->setText(editorFormat->path);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(73));
		positionInput->setText(editorFormat->texture);
		positionInput = dynamic_cast<Input*>(m_uiManager->getById(74));
		positionInput->setText(editorFormat->extraParams[0]);
	}

	void updateSky()
	{
		if (!m_selectedModel) {
			return;
		}

		Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(71));
		std::string title = "Sky properties - ";
		title += std::to_string(m_selectedModel->getId());
		objectWindow->setTitle(title);

		Input* input;
		std::string path, textures, texturesPertube;

		input = dynamic_cast<Input*>(m_uiManager->getById(72));
		path = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(73));
		textures = input->getValue();
		input = dynamic_cast<Input*>(m_uiManager->getById(74));
		texturesPertube = input->getValue();


		MapEntity::ObjectFormat* editorFormat = getObjectEditor(m_selectedModel->getId());
		editorFormat->path = path;
		editorFormat->texture = textures;
		editorFormat->extraParams[0] = texturesPertube;

		SkyPlaneClass* model = dynamic_cast<SkyPlaneClass*>(m_selectedModel);
		SkyDomeClass* modelDome = dynamic_cast<SkyDomeClass*>(m_modelManager->getById(stoi(editorFormat->extraParams[1])));

		model->Shutdown();
		model->Initialize(m_Graphics->getD3D(), { &textures[0], &texturesPertube[0] });
		modelDome->Shutdown();
		modelDome->Initialize(m_Graphics->getD3D(), path);
	}

	void updateBbox()
	{
		D3DXVECTOR3 direction, position, scale, rotation;
		if (m_selectedModel) {
			BBox* bbox = dynamic_cast<BBox*>(m_modelManager->getById(2));

			m_selectedModel->GetBoundingBox(position, scale);

			bbox->Shutdown();
			bbox->CreateBox(m_Graphics->getD3D(), position, scale);
			bbox->setVisible(true);
		}
	}*/


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	App* app = new App;

	if (app->init()) {
		app->run();
	}

	app->shutdown();
	delete app;
	app = 0;

	return 0;
}