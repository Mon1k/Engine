#pragma once

#include "../main.h"

#include "../../../Engine/reader/xml/XmlWriter.h"
#include "../../../Engine/reader/xml/XmlReader.h"
#include "../../../Engine/map/XmlSave.h"
#include "../../../Engine/map/XmlLoad.h"

#include "../../../Engine/ui/dialogs/SaveDialog.h"
#include "../../../Engine/ui/dialogs/Confirm.h"

#include "ObjectWindow.h"
#include "TerrainWindow.h"
#include "WaterWindow.h"
#include "SkyWindow.h"

class MainWindow
{
public:
	MainWindow::MainWindow(App* app)
	{
		m_app = app;
	}

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
		exitButton->Add("New", 1, 1);
		exitButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			Confirm* dialog = new Confirm;
			m_app->m_uiManager->add(dialog);
			dialog->showText("Clear scene?");
			dialog->addEventHandler(AbstractGui::EventType::ACTION_CHOOSE, [this, dialog] {
				if (dialog->getResult()) {
					m_app->m_modelManager->clear();
					m_app->m_mapEntities->clear();
					m_app->initDefaultObjects();
				}
				m_app->m_uiManager->remove(dialog->getId());
			});
		});

		Button* saveWorldButton = new Button;
		menuTop->addChild(saveWorldButton);
		saveWorldButton->initialize(80, 28);
		saveWorldButton->Add("Save", 82, 1);
		saveWorldButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			SaveDialog* dialog = new SaveDialog;
			m_app->m_uiManager->add(dialog);
			dialog->initialize();
			dialog->addFilter("map");
			dialog->setId(m_app->m_uiManager->getNextId());
			dialog->addEventHandler(FileChooser::EventType::FILE_CHOOSE_GET, [this, dialog] {
				dialog->hide();
				m_app->m_uiManager->remove(dialog->getId());
				XmlSave* xmlSave = new XmlSave(new XmlWriter);
				xmlSave->save(dialog->getFilename(), m_app->m_mapEntities);
				m_app->initDefaultObjects();
			});
			dialog->show();
			dialog->proccesedEventHandlers(Window::EventType::WINDOW_OPEN);
		});

		Button* loadWorldButton = new Button;
		menuTop->addChild(loadWorldButton);
		loadWorldButton->initialize(80, 28);
		loadWorldButton->Add("Load", 164, 1);
		loadWorldButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			OpenDialog* dialog = new OpenDialog;
			m_app->m_uiManager->add(dialog);
			dialog->initialize();
			dialog->addFilter("map");
			dialog->setId(m_app->m_uiManager->getNextId());
			dialog->addEventHandler(FileChooser::EventType::FILE_CHOOSE, [this, dialog] {
				dialog->hide();
				m_app->m_uiManager->remove(dialog->getId());
				XmlLoad* xmlLoad = new XmlLoad(new XmlReader);
				xmlLoad->load(dialog->getCurrentFilePath(), m_app->m_mapEntities, m_app->m_modelManager);
				m_app->initDefaultObjects();
			});
			dialog->show();
			dialog->proccesedEventHandlers(Window::EventType::WINDOW_OPEN);
		});

		Button* newObjectButton = new Button;
		menuTop->addChild(newObjectButton);
		newObjectButton->initialize(120, 28);
		newObjectButton->Add("New Object", 246, 1);
		newObjectButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			m_app->getObjectWindow()->resetUI();
			m_app->getObjectWindow()->getWindow()->show();
		});

		Button* newCompositeObjectButton = new Button;
		menuTop->addChild(newCompositeObjectButton);
		newCompositeObjectButton->initialize(170, 28);
		newCompositeObjectButton->Add("New Composite Object", 368, 1);
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
		newTerrainButton->Add("New Terrain", 540, 1);
		newTerrainButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			m_app->getTerrainWindow()->resetUI();
			m_app->getTerrainWindow()->getWindow()->show();
		});

		Button* newWaterButton = new Button;
		menuTop->addChild(newWaterButton);
		newWaterButton->initialize(120, 28);
		newWaterButton->Add("New Water", 662, 1);
		newWaterButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			m_app->getWaterWindow()->resetUI();
			m_app->getWaterWindow()->getWindow()->show();
		});

		Button* newSkyButton = new Button;
		menuTop->addChild(newSkyButton);
		newSkyButton->initialize(120, 28);
		newSkyButton->Add("New Sky", 784, 1);
		newSkyButton->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_app->unselectModel();
			m_app->getSkyWindow()->resetUI();
			m_app->getSkyWindow()->getWindow()->show();
		});
	}

protected:
	App* m_app;
};