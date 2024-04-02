#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/checkbox.h"

class ObjectWindow
{
public:
	void initialize()
	{
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
			return 0;
			});
		shift += objectWindow->m_y + objectWindow->getHeader()->m_height + 5;

		Input* objectPath = new Input;
		objectWindow->addChild(objectPath);
		objectPath->initialize(400, 28, objectWindow->m_x + 10, shift);
		objectPath->setText("data/models/bath.ds");
		objectPath->setId(19);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectPath, this] {
			if (!m_app->m_selectedModel) {
				//m_app->resetWindowModel();
				Input* objectTexture = dynamic_cast<Input*>(m_app->m_uiManager->getById(20));

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

					AbstractGui* objectWindow = m_app->m_uiManager->getById(2);
					objectWindow->show();
					objectWindow->focus();

					MapEntity::ObjectFormat format;
					format.id = model->getId();
					format.type = MapEntity::ObjectTypes::MODEL;
					format.position = model->GetPosition();
					format.scale = model->GetScale();
					format.rotation = model->getRotation();
					format.path = objectPath->getValue();
					format.texture = objectTexture->getValue();
					format.parent = 0;
					// is alpha
					format.extraParams.push_back("0");
					m_app->m_mapEntities->add(format);

					//this->updateObjectModel();
				}
			}
			else {
				MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
				editorFormat->path = objectPath->getValue();
			}
			return 0;
			});
		shift += objectPath->m_height + 5;


		Input* objectTexture = new Input;
		objectWindow->addChild(objectTexture);
		objectTexture->initialize(400, 28, objectWindow->m_x + 10, shift);
		objectTexture->setText("data/textures/wall01.dds");
		objectTexture->setId(20);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			if (m_app->m_selectedModel) {
				m_app->m_selectedModel->ReleaseTexture();
				m_app->m_selectedModel->LoadTextures(objectTexture->getValue());

				MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
				editorFormat->texture = objectTexture->getValue();

				//this->updateObjectModel();
			}
			return 0;
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
		objectPositionX->setText("0.00");
		objectPositionX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objectPositionX, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 position = m_app->m_selectedModel->GetPosition();
				position.x = std::stof(objectPositionX->getValue());
				m_app->m_selectedModel->SetPosition(position);

				//this->updateObjectModel();
			}
			return 0;
			});
		Input* objectPositionY = new Input;
		objectWindow->addChild(objectPositionY);
		objectPositionY->initialize(100, 28, objectWindow->m_x + 112, shift);
		objectPositionY->setId(11);
		objectPositionY->setText("0.00");
		objectPositionY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objectPositionY, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 position = m_app->m_selectedModel->GetPosition();
				position.y = std::stof(objectPositionY->getValue());
				m_app->m_selectedModel->SetPosition(position);
				//this->updateObjectModel();
			}
			return 0;
			});
		Input* objectPositionZ = new Input;
		objectWindow->addChild(objectPositionZ);
		objectPositionZ->initialize(100, 28, objectWindow->m_x + 214, shift);
		objectPositionZ->setId(12);
		objectPositionZ->setText("0.00");
		objectPositionZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objectPositionZ, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 position = m_app->m_selectedModel->GetPosition();
				position.z = std::stof(objectPositionZ->getValue());
				m_app->m_selectedModel->SetPosition(position);
				//this->updateObjectModel();
			}
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
		objectScaleX->initialize(100, 28, objectWindow->m_x + 10, shift);
		objectScaleX->setId(13);
		objectScaleX->setText("1.00");
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleX, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 scale = m_app->m_selectedModel->GetScale();
				scale.x = std::stof(objectScaleX->getValue());
				m_app->m_selectedModel->SetScale(scale);
				//this->updateObjectModel();
			}
			return 0;
			});
		Input* objectScaleY = new Input;
		objectWindow->addChild(objectScaleY);
		objectScaleY->initialize(100, 28, objectWindow->m_x + 112, shift);
		objectScaleY->setId(14);
		objectScaleY->setText("1.00");
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleY, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 scale = m_app->m_selectedModel->GetScale();
				scale.y = std::stof(objectScaleY->getValue());
				m_app->m_selectedModel->SetScale(scale);
				//this->updateObjectModel();
			}
			return 0;
			});
		Input* objectScaleZ = new Input;
		objectWindow->addChild(objectScaleZ);
		objectScaleZ->initialize(100, 28, objectWindow->m_x + 214, shift);
		objectScaleZ->setId(15);
		objectScaleZ->setText("1.00");
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectScaleZ, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 scale = m_app->m_selectedModel->GetScale();
				scale.z = std::stof(objectScaleZ->getValue());
				m_app->m_selectedModel->SetScale(scale);
				//this->updateObjectModel();
			}
			return 0;
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
		objecRotateX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objecRotateX, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 rotation = m_app->m_selectedModel->getRotation();
				rotation.x = std::stof(objecRotateX->getValue());
				m_app->m_selectedModel->SetRotation(rotation);
				//this->updateObjectModel();
			}
			return 0;
			});
		Input* objecRotateY = new Input;
		objectWindow->addChild(objecRotateY);
		objecRotateY->initialize(100, 28, objectWindow->m_x + 112, shift);
		objecRotateY->setId(17);
		objecRotateY->setText("0.00");
		objecRotateY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objecRotateY, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 rotation = m_app->m_selectedModel->getRotation();
				rotation.y = std::stof(objecRotateY->getValue());
				m_app->m_selectedModel->SetRotation(rotation);
				//this->updateObjectModel();
			}
			return 0;
			});
		Input* objecRotateZ = new Input;
		objectWindow->addChild(objecRotateZ);
		objecRotateZ->initialize(100, 28, objectWindow->m_x + 214, shift);
		objecRotateZ->setId(18);
		objecRotateZ->setText("0.00");
		objecRotateZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [objecRotateZ, this] {
			if (m_app->m_selectedModel) {
				D3DXVECTOR3 rotation = m_app->m_selectedModel->getRotation();
				rotation.z = std::stof(objecRotateZ->getValue());
				m_app->m_selectedModel->SetRotation(rotation);
				//this->updateObjectModel();
			}
			return 0;
			});
		shift += objecRotateX->m_height + 5;


		Checkbox* alphaObject = new Checkbox;
		objectWindow->addChild(alphaObject);
		alphaObject->initialize();
		alphaObject->Add("Is alpha", objectWindow->m_x + 10, shift);
		alphaObject->setId(75);
		alphaObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			if (m_app->m_selectedModel) {
				//this->updateObjectModel();
			}
			return 0;
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
			return 0;
			});
	}

public:
	App* m_app;
};