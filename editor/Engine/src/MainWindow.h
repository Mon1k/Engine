#pragma once

#include "../main.h"

#include "../../Engine/reader/xml/XmlWriter.h"
#include "../../Engine/reader/xml/XmlReader.h"
#include "../../Engine/map/XmlSave.h"
#include "../../Engine/map/XmlLoad.h"

class MainWindow
{
public:
	void initialize()
	{
		Window* menuTop = new Window;
		m_app->m_uiManager->add(menuTop);
		menuTop->Initialize(Options::screen_width, 31, 0, 0);
		menuTop->addBody();
		menuTop->setId(1);


		Button* exitButton = new Button;
		menuTop->addChild(exitButton);
		exitButton->initialize(80, 28);
		exitButton->Add("Exit", 1, 1);
		exitButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->m_Done = true;
			return 0;
			});

		Button* saveWorldButton = new Button;
		menuTop->addChild(saveWorldButton);
		saveWorldButton->initialize(80, 28);
		saveWorldButton->Add("Save", 82, 1);
		saveWorldButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			XmlSave* xmlSave = new XmlSave(new XmlWriter);
			xmlSave->save("main.map", m_app->m_mapEntities);
			return 0;
			});

		Button* loadWorldButton = new Button;
		menuTop->addChild(loadWorldButton);
		loadWorldButton->initialize(80, 28);
		loadWorldButton->Add("Load", 164, 1);
		loadWorldButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			XmlLoad* xmlLoad = new XmlLoad(new XmlReader);
			xmlLoad->load("main.map", m_app->m_mapEntities, m_app->m_modelManager);
			m_app->initDefaultObjects();
			return 0;
			});

		Button* newObjectButton = new Button;
		menuTop->addChild(newObjectButton);
		newObjectButton->initialize(120, 28);
		newObjectButton->Add("New Object Node", 246, 1);
		newObjectButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this, menuTop] {
			//this->resetWindowModel();
			m_app->unselectModel();
			Window* objectWindow = dynamic_cast<Window*>(m_app->m_uiManager->getById(2));
			objectWindow->show();
			objectWindow->focus();
		});

		Button* newCompositeObjectButton = new Button;
		menuTop->addChild(newCompositeObjectButton);
		newCompositeObjectButton->initialize(170, 28);
		newCompositeObjectButton->Add("New Composite Object Node", 368, 1);
		newCompositeObjectButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			/*this->resetWindowCompositeModel();
			unselectModel();
			Window* objectWindow = dynamic_cast<Window*>(m_uiManager->getById(41));
			objectWindow->show();
			objectWindow->focus();*/
		});

		Button* newTerrainButton = new Button;
		menuTop->addChild(newTerrainButton);
		newTerrainButton->initialize(120, 28);
		newTerrainButton->Add("New Terrain Node", 540, 1);
		newTerrainButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			/*resetWindowTerrain();
			unselectModel();
			Window* terrainWindow = dynamic_cast<Window*>(m_uiManager->getById(21));
			terrainWindow->show();
			terrainWindow->focus();*/
		});

		Button* newWaterButton = new Button;
		menuTop->addChild(newWaterButton);
		newWaterButton->initialize(120, 28);
		newWaterButton->Add("New Water Node", 662, 1);
		newWaterButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			/*resetWindowWater();
			unselectModel();
			Window* waterWindow = dynamic_cast<Window*>(m_uiManager->getById(51));
			waterWindow->show();
			waterWindow->focus();*/
		});

		Button* newSkyButton = new Button;
		menuTop->addChild(newSkyButton);
		newSkyButton->initialize(120, 28);
		newSkyButton->Add("New Sky Node", 784, 1);
		newSkyButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			/*resetWindowSky();
			unselectModel();
			Window* skyWindow = dynamic_cast<Window*>(m_uiManager->getById(71));
			skyWindow->show();
			skyWindow->focus();*/
		});
	}

public:
	App* m_app;
};