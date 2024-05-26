#include "main.h"

#include "../../Engine/ui/label.h"
#include "../../Engine/ui/cursor.h"

#include "../../Engine/models/gridclass.h"
#include "../../Engine/lightclass.h"

#include "../../Engine/CollisionDetection.h"
#include "../../Engine/tool/String.h"

#include "src/MainWindow.h"
#include "src/ObjectWindow.h"
#include "src/TerrainWindow.h"
#include "src/WaterWindow.h"
#include "src/SkyWindow.h"

void App::initDefaultObjects()
{
	GridClass* grid = new GridClass;
	grid->Initialize(m_Graphics->getD3D(), 100, 100);
	grid->setId(1);
	m_modelManager->Add(grid);
}

void App::InitMenuTop()
{
	m_mainWindow = new MainWindow(this);
	m_mainWindow->initialize();
}

ObjectWindow* App::getObjectWindow()
{
	if (!m_objectWindow) {
		m_objectWindow = new ObjectWindow(this);
		m_objectWindow->initialize();
	}

	return m_objectWindow;
}

TerrainWindow* App::getTerrainWindow()
{
	if (!m_terrainWindow) {
		m_terrainWindow = new TerrainWindow(this);
		m_terrainWindow->initialize();
	}

	return m_terrainWindow;
}

WaterWindow* App::getWaterWindow()
{
	if (!m_waterWindow) {
		m_waterWindow = new WaterWindow(this);
		m_waterWindow->initialize();
	}

	return m_waterWindow;
}

SkyWindow* App::getSkyWindow()
{
	if (!m_skyWindow) {
		m_skyWindow = new SkyWindow(this);
		m_skyWindow->initialize();
	}

	return m_skyWindow;
}

	/*void InitWindowCompositeObject()
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
	*/

bool App::init()
{
	if (!SystemClass::init()) {
		return false;
	}

	m_selectedModel = 0;
	m_objectWindow = 0;
	m_terrainWindow = 0;
	m_waterWindow = 0;
	m_skyWindow = 0;

	int size = sizeof(m_Counters) / sizeof(m_Counters[0]);
	for (int i = 0; i < size; i++) {
		m_Counters[i] = 0;
	}

	m_Position->SetPosition(8.0f, 10.0f, 30.0f);
	//m_Position->SetPosition(222.0f, 20.0f, 200.0f);
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

	Cursor* cursor = new Cursor;
	m_uiManager->add(cursor);
	cursor->Initialize();
	cursor->setId(6);

	this->InitMenuTop();
}

void App::loadScene()
{
	/* @todo - later as light entity, also sync with light from xmlload */
	m_light = new LightClass;
	m_light->SetAmbientColor(0.35f, 0.35f, 0.35f, 1.0f);
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -1.0f, 1.0f);
	m_light->SetPosition(0.0f, 220.0f, 0.0f);
	m_light->SetLookAt(300.0f, 0.0f, 200.0f);
	m_light->setIntensity(1.0f);
	m_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetSpecularPower(32.0f);
	m_light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

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

	if (m_Input->IsKeyDown(DIK_ESCAPE)) {
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

			MapEntity::ObjectFormat* entity = m_mapEntities->getEntity(models[i]->getId());
			if (!entity || entity->type != MapEntity::MODEL) {
				continue;
			}

			ModelClass* model = dynamic_cast<ModelClass*>(models[i]);

			m_CollisionDetection->getRayDirection(m_Graphics->getD3D()->getScreenWidth(), m_Graphics->getD3D()->getScreenHeight(), mouseX, mouseY, position, direction);
			if (m_CollisionDetection->RayBoxIntersect(position, direction, model->getMinPosition(), model->getMaxPosition())) {
				modelsSelected.push_back(model);
			}
		}
		if (modelsSelected.size() == 0) {
			for (int i = 0; i < models.size(); i++) {
				MapEntity::ObjectFormat* entity = m_mapEntities->getEntity(models[i]->getId());
				if (!entity || entity->type == MapEntity::MODEL) {
					continue;
				}

				ModelClass* model = dynamic_cast<ModelClass*>(models[i]);
				m_CollisionDetection->getRayDirection(m_Graphics->getD3D()->getScreenWidth(), m_Graphics->getD3D()->getScreenHeight(), mouseX, mouseY, position, direction);
				if (m_CollisionDetection->RayBoxIntersect(position, direction, model->getMinPosition(), model->getMaxPosition())) {
					modelsSelected.push_back(model);
				}
			}
		}

		ModelClass* modelNear = 0;
		float minDistance = D3D11_FLOAT32_MAX;
		float distance;
		for (int i = 0; i < modelsSelected.size(); i++) {
			ModelClass* model = dynamic_cast<ModelClass*>(modelsSelected[i]);
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
				this->getObjectWindow()->updateUiFromModel();
			}
			/*else if (editorFormat->type == MapEntity::ObjectTypes::COMPOSITE_MODEL) {
				this->updateCompositeModel();
			}*/
			else if (editorFormat->type == MapEntity::ObjectTypes::TERRAIN) {
				this->getTerrainWindow()->updateUiFromModel();
			}
			else if (editorFormat->type == MapEntity::ObjectTypes::WATER) {
				this->getWaterWindow()->updateUiFromModel();
			}
			else if (editorFormat->type == MapEntity::ObjectTypes::SKY) {
				this->getSkyWindow()->updateUiFromModel();
			}
		}
	}

	std::vector<AbstractGui*> elements = m_uiManager->getElements();
	for (size_t i = 0; i < elements.size(); i++) {
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

		if (this->m_objectWindow) {
			this->getObjectWindow()->getWindow()->hide();
		}
		if (this->m_terrainWindow) {
			this->getTerrainWindow()->getWindow()->hide();
		}
		if (this->m_waterWindow) {
			this->getWaterWindow()->getWindow()->hide();
		}
		if (this->m_skyWindow) {
			this->getSkyWindow()->getWindow()->hide();
		}
	}
}

	/*
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
	*/


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