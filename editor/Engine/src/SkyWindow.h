#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/FileInput.h"

#include "../../../Engine/models/sky/skydomeclass.h"

class SkyWindow
{
public:
	SkyWindow::SkyWindow(App* app)
	{
		m_app = app;
		resetUI();
	}

	void resetUI()
	{
		m_path = "data/models/skydome.ds";

		m_apexColor = D3DXVECTOR4(0.49f, 0.81f, 1.0f, 1.0f);
		m_centerColor = D3DXVECTOR4(0.7f, 0.87f, 1.0f, 1.0f);
	}

	void initialize()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_app->m_uiManager->getById(1));

		m_Window = new Window;
		m_app->m_uiManager->add(m_Window);
		m_Window->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		m_Window->addHeader("Sky properties", Window::HEADER_BUTTON_CLOSE);
		m_Window->addBody();
		m_Window->hide();
		m_Window->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			m_app->unselectModel();
		});
		shift += m_Window->m_y + m_Window->getHeader()->m_height + 5;

		FileInput* objectPath = new FileInput;
		m_Window->addChild(objectPath);
		objectPath->initialize(400, 28, m_Window->m_x + 10, shift);
		objectPath->getDialog()->setPath(objectPath->getDialog()->getCurrentPath() + "data/models");
		objectPath->getDialog()->addDefaultModelsFilters();
		objectPath->setValueRefLink(&typeid(std::string), &m_path);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			if (!m_app->m_selectedModel) {
				Input* objectTexture1 = dynamic_cast<Input*>(m_app->m_uiManager->getById(73));
				Input* objectTexture2 = dynamic_cast<Input*>(m_app->m_uiManager->getById(74));

				SkyDomeClass* model = new SkyDomeClass;
				model->Initialize(m_app->getGraphic()->getD3D(), m_path);
				model->setId(m_app->m_modelManager->getNextId());
				
				m_app->m_modelManager->Add(model);
				m_app->m_selectedModel = model;

				MapEntity::ObjectFormat format;
				format.id = model->getId();
				format.name = "Sky " + std::to_string(format.id);
				format.type = MapEntity::ObjectTypes::SKY;
				format.path = m_path;

				format.params.insert(std::pair<std::string, std::string>("apex_color", ""));
				format.params.insert(std::pair<std::string, std::string>("center_color", ""));

				m_app->m_mapEntities->add(format);
			}
			this->updateModel();
		});
		shift += objectPath->m_height + 5;


		Label* m_WindowLabelApexColor = new Label;
		m_Window->addChild(m_WindowLabelApexColor);
		m_WindowLabelApexColor->Initialize(50, 20);
		m_WindowLabelApexColor->Add("Apex color", m_Window->m_x + 5, shift);
		shift += m_WindowLabelApexColor->m_height + 1;

		Input* apexColorX = new Input;
		m_Window->addChild(apexColorX);
		apexColorX->initialize(100, 28, m_Window->m_x + 10, shift);
		apexColorX->setValueRefLink(&typeid(float), &m_apexColor.x);
		apexColorX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateModel();
		});
		Input* apexColorY = new Input;
		m_Window->addChild(apexColorY);
		apexColorY->initialize(100, 28, m_Window->m_x + 112, shift);
		apexColorY->setValueRefLink(&typeid(float), &m_apexColor.y);
		apexColorY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateModel();
		});
		Input* apexColorZ = new Input;
		m_Window->addChild(apexColorZ);
		apexColorZ->initialize(100, 28, m_Window->m_x + 214, shift);
		apexColorZ->setValueRefLink(&typeid(float), &m_apexColor.z);
		apexColorZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateModel();
		});
		shift += apexColorZ->m_height + 5;


		Label* m_WindowLabelCenterColor = new Label;
		m_Window->addChild(m_WindowLabelCenterColor);
		m_WindowLabelCenterColor->Initialize(50, 20);
		m_WindowLabelCenterColor->Add("Center color", m_Window->m_x + 5, shift);
		shift += m_WindowLabelCenterColor->m_height + 1;

		Input* centerColorX = new Input;
		m_Window->addChild(centerColorX);
		centerColorX->initialize(100, 28, m_Window->m_x + 10, shift);
		centerColorX->setValueRefLink(&typeid(float), &m_centerColor.x);
		centerColorX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateModel();
		});
		Input* centerColorY = new Input;
		m_Window->addChild(centerColorY);
		centerColorY->initialize(100, 28, m_Window->m_x + 112, shift);
		centerColorY->setValueRefLink(&typeid(float), &m_centerColor.y);
		centerColorY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateModel();
		});
		Input* centerColorZ = new Input;
		m_Window->addChild(centerColorZ);
		centerColorZ->initialize(100, 28, m_Window->m_x + 214, shift);
		centerColorZ->setValueRefLink(&typeid(float), &m_centerColor.z);
		centerColorZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateModel();
		});
		shift += centerColorZ->m_height + 5;
	}

	void updateUiFromModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		SkyDomeClass* model = dynamic_cast<SkyDomeClass*>(m_app->m_selectedModel);

		m_path = model->getPath();
		m_apexColor = model->GetApexColor();
		m_centerColor = model->GetCenterColor();

		m_Window->setTitle("Sky properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();
	}

	void updateModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		m_Window->setTitle("Sky properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();

		SkyDomeClass* model = dynamic_cast<SkyDomeClass*>(m_app->m_selectedModel);
		model->Shutdown();
		model->Initialize(m_app->getGraphic()->getD3D(), m_path);
		model->setApexColor(m_apexColor);
		model->setCenterColor(m_centerColor);

		MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
		editorFormat->path = m_path;

		editorFormat->params["apex_color"] = std::to_string(m_apexColor.x)+";"+ std::to_string(m_apexColor.y) + ";"+ std::to_string(m_apexColor.z) + ";"+ std::to_string(m_apexColor.w);
		editorFormat->params["center_color"] = std::to_string(m_centerColor.x)+";"+ std::to_string(m_centerColor.y) + ";"+ std::to_string(m_centerColor.z) + ";"+ std::to_string(m_centerColor.w);
		
	}

	Window* getWindow()
	{
		return m_Window;
	}


protected:
	App* m_app;
	Window* m_Window;

	std::string m_path;
	D3DXVECTOR4 m_apexColor;
	D3DXVECTOR4 m_centerColor;
};
