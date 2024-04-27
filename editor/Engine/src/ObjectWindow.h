#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/checkbox.h"
#include "../../../Engine/ui/FileInput.h"

class ObjectWindow
{
public:
	void resetUI()
	{
		m_path = "data/models/bath.ds";
		m_texture = "data/textures/wall01.dds";

		m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_isAlpha = false;
	}

	void initialize()
	{
		resetUI();

		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_app->m_uiManager->getById(1));

		Window* objectWindow = new Window;
		m_app->m_uiManager->add(objectWindow);
		objectWindow->Initialize(600, 400, Options::screen_width - 620, menuTop->m_height + 5);
		objectWindow->addHeader("Object properties", Window::HEADER_BUTTON_CLOSE);
		objectWindow->addBody();
		objectWindow->hide();
		objectWindow->setId(2);
		objectWindow->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			m_app->unselectModel();
		});
		shift += objectWindow->m_y + objectWindow->getHeader()->m_height + 5;

		FileInput* objectPath = new FileInput;
		FileInput* objectTexture = new FileInput;

		objectWindow->addChild(objectPath);
		objectPath->initialize(400, 28, objectWindow->m_x + 10, shift);
		objectPath->getDialog()->setPath(objectPath->getDialog()->getCurrentPath() + "/data/models");
		objectPath->getDialog()->addDefaultModelsFilters();
		objectPath->setId(19);
		objectPath->setValueRefLink(&typeid(std::string), &m_path);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this, objectPath, objectTexture] {
			if (!m_app->m_selectedModel) {
				Model* model = new Model;
				bool result = model->Initialize(m_app->getGraphic()->getD3D(), &objectPath->getValue()[0], {objectTexture->getValue()});
				if (result) {
					model->setId(m_app->m_modelManager->getNextId());
					model->addLights({ m_app->m_light });
					LightShaderClass* shader = new LightShaderClass;
					shader->Initialize(m_app->getGraphic()->getD3D()->GetDevice());
					shader->addLights({ m_app->m_light });
					model->addShader(shader);

					m_app->m_modelManager->Add(model);
					m_app->m_selectedModel = model;

					MapEntity::ObjectFormat format;
					format.model = model;
					format.parent = 0;

					format.id = model->getId();
					format.type = MapEntity::ObjectTypes::MODEL;
					format.position = model->GetPosition();
					format.scale = model->GetScale();
					format.rotation = model->getRotation();
					format.path = objectPath->getValue();
					format.texture = objectTexture->getValue();

					format.params.insert(std::pair<std::string, std::string>("alpha", "0"));
					
					m_app->m_mapEntities->add(format);
				}
			}
			else {
				Model* model = m_app->m_selectedModel;
				model->Shutdown();
				model->addLights({ m_app->m_light });
				LightShaderClass* shader = new LightShaderClass;
				shader->Initialize(m_app->getGraphic()->getD3D()->GetDevice());
				shader->addLights({ m_app->m_light });
				model->addShader(shader);
				model->Initialize(m_app->getGraphic()->getD3D(), &objectPath->getValue()[0], { objectTexture->getValue() });
			}
			this->updateObjectModel();
		});
		shift += objectPath->m_height + 5;

		objectWindow->addChild(objectTexture);
		objectTexture->initialize(400, 28, objectWindow->m_x + 10, shift);
		objectTexture->getDialog()->setPath(objectTexture->getDialog()->getCurrentPath() + "/data/textures");
		objectTexture->getDialog()->addDefaultImageFilters();
		objectTexture->setId(20);
		objectTexture->setValueRefLink(&typeid(std::string), &m_texture);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			if (m_app->m_selectedModel) {
				m_app->m_selectedModel->ReleaseTexture();
				m_app->m_selectedModel->LoadTextures(objectTexture->getValue());
				this->updateObjectModel();
			}
		});
		shift += objectTexture->m_height + 5;


		Label* objectWindowLabelPosition = new Label;
		objectWindow->addChild(objectWindowLabelPosition);
		objectWindowLabelPosition->Initialize(50, 20);
		objectWindowLabelPosition->Add("Position", objectWindow->m_x + 5, shift);
		shift += objectWindowLabelPosition->m_height + 1;

		Input* objectPositionX = new Input;
		objectWindow->addChild(objectPositionX);
		objectPositionX->initialize(100, 28, objectWindow->m_x + 10, shift);
		objectPositionX->setId(10);
		objectPositionX->setText("0.0");
		objectPositionX->setValueRefLink(&typeid(float), &m_position.x);
		objectPositionX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objectPositionX, this] {
			this->updateObjectModel();
		});
		Input* objectPositionY = new Input;
		objectWindow->addChild(objectPositionY);
		objectPositionY->initialize(100, 28, objectWindow->m_x + 112, shift);
		objectPositionY->setId(11);
		objectPositionY->setText("0.0");
		objectPositionY->setValueRefLink(&typeid(float), &m_position.y);
		objectPositionY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objectPositionY, this] {
			this->updateObjectModel();
		});
		Input* objectPositionZ = new Input;
		objectWindow->addChild(objectPositionZ);
		objectPositionZ->initialize(100, 28, objectWindow->m_x + 214, shift);
		objectPositionZ->setId(12);
		objectPositionZ->setText("0.0");
		objectPositionZ->setValueRefLink(&typeid(float), &m_position.z);
		objectPositionZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objectPositionZ, this] {
			this->updateObjectModel();
		});
		shift += objectPositionX->m_height + 5;


		Label* objectWindowLabelScale = new Label;
		objectWindow->addChild(objectWindowLabelScale);
		objectWindowLabelScale->Initialize(50, 20);
		objectWindowLabelScale->Add("Scale", objectWindow->m_x + 5, shift);
		shift += objectWindowLabelScale->m_height + 1;

		Input* objectScaleX = new Input;
		objectWindow->addChild(objectScaleX);
		objectScaleX->initialize(100, 28, objectWindow->m_x + 10, shift);
		objectScaleX->setId(13);
		objectScaleX->setText("1.00");
		objectPositionZ->setValueRefLink(&typeid(float), &m_scale.x);
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleX, this] {
			this->updateObjectModel();
		});
		Input* objectScaleY = new Input;
		objectWindow->addChild(objectScaleY);
		objectScaleY->initialize(100, 28, objectWindow->m_x + 112, shift);
		objectScaleY->setId(14);
		objectScaleY->setText("1.00");
		objectScaleY->setValueRefLink(&typeid(float), &m_scale.y);
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleY, this] {
			this->updateObjectModel();
		});
		Input* objectScaleZ = new Input;
		objectWindow->addChild(objectScaleZ);
		objectScaleZ->initialize(100, 28, objectWindow->m_x + 214, shift);
		objectScaleZ->setId(15);
		objectScaleZ->setText("1.00");
		objectScaleZ->setValueRefLink(&typeid(float), &m_scale.z);
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleZ, this] {
			this->updateObjectModel();
		});
		shift += objectScaleX->m_height + 5;


		Label* objectWindowLabelRotate = new Label;
		objectWindow->addChild(objectWindowLabelRotate);
		objectWindowLabelRotate->Initialize(50, 20);
		objectWindowLabelRotate->Add("Rotation", objectWindow->m_x + 5, shift);
		shift += objectWindowLabelRotate->m_height + 1;

		Input* objecRotateX = new Input;
		objectWindow->addChild(objecRotateX);
		objecRotateX->initialize(100, 28, objectWindow->m_x + 10, shift);
		objecRotateX->setId(16);
		objecRotateX->setText("0.00");
		objecRotateX->setValueRefLink(&typeid(float), &m_rotation.x);
		objecRotateX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objecRotateX, this] {
			this->updateObjectModel();
		});
		Input* objecRotateY = new Input;
		objectWindow->addChild(objecRotateY);
		objecRotateY->initialize(100, 28, objectWindow->m_x + 112, shift);
		objecRotateY->setId(17);
		objecRotateY->setText("0.00");
		objecRotateY->setValueRefLink(&typeid(float), &m_rotation.y);
		objecRotateY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objecRotateY, this] {
			this->updateObjectModel();
		});
		Input* objecRotateZ = new Input;
		objectWindow->addChild(objecRotateZ);
		objecRotateZ->initialize(100, 28, objectWindow->m_x + 214, shift);
		objecRotateZ->setId(18);
		objecRotateZ->setText("0.00");
		objecRotateZ->setValueRefLink(&typeid(float), &m_rotation.z);
		objecRotateZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objecRotateZ, this] {
			this->updateObjectModel();
		});
		shift += objecRotateX->m_height + 5;


		Checkbox* alphaObject = new Checkbox;
		objectWindow->addChild(alphaObject);
		alphaObject->initialize();
		alphaObject->Add("Is alpha", objectWindow->m_x + 10, shift);
		alphaObject->setId(75);
		alphaObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			//this->updateObjectModel();
		});
		shift += alphaObject->m_height + 5;


		Button* cloneObject = new Button;
		objectWindow->addChild(cloneObject);
		cloneObject->initialize(100, 20);
		cloneObject->Add("Clone", objectWindow->m_x + objectWindow->m_width - 110, objectWindow->m_y + objectWindow->m_height - 30);
		cloneObject->setId(76);
		cloneObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [objectWindow, this] {
			if (m_app->m_selectedModel) {
				MapEntity::ObjectFormat* entity = m_app->m_mapEntities->getEntity(m_app->m_selectedModel->getId());
				m_app->m_mapEntities->copyModel(*entity, m_app->m_modelManager);

				m_app->unselectModel();
				objectWindow->hide();
			}
		});
	}

	void updateUiFromModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		m_path = m_app->m_selectedModel->getPath();
		m_texture = m_app->m_selectedModel->GetTextureClass()->getTexturePath();

		m_position = m_app->m_selectedModel->GetPosition();
		m_scale = m_app->m_selectedModel->GetScale();
		m_rotation = m_app->m_selectedModel->getRotationDegree();
		m_isAlpha = m_app->m_selectedModel->getAlpha();

		Window* objectWindow = dynamic_cast<Window*>(m_app->m_uiManager->getById(2));
		objectWindow->setTitle("Object properties - " + std::to_string(m_app->m_selectedModel->getId()));
		objectWindow->focus();
		objectWindow->show();
	}

	void updateObjectModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		Window* objectWindow = dynamic_cast<Window*>(m_app->m_uiManager->getById(2));
		objectWindow->setTitle("Object properties - " + std::to_string(m_app->m_selectedModel->getId()));
		objectWindow->focus();
		objectWindow->show();

		Checkbox* checkbox = dynamic_cast<Checkbox*>(m_app->m_uiManager->getById(75));
		m_isAlpha = checkbox->getIsMarked();


		m_app->m_selectedModel->SetPosition(m_position);
		m_app->m_selectedModel->SetScale(m_scale);
		m_app->m_selectedModel->SetRotation(m_rotation);
		m_app->m_selectedModel->setAlpha(m_isAlpha);
		m_app->m_selectedModel->hideBBox();
		m_app->m_selectedModel->showBBox();

		MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
		editorFormat->path = m_path;
		editorFormat->texture = m_texture;
		editorFormat->position = m_position;
		editorFormat->scale = m_scale;
		editorFormat->rotation = m_rotation;
		editorFormat->params["alpha"] = std::to_string(m_isAlpha ? 1 : 0);
	}

public:
	App* m_app;

	std::string m_path;
	std::string m_texture;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_scale;
	D3DXVECTOR3 m_rotation;
	bool m_isAlpha;
};