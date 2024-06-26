#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/dialogs/window.h"
#include "../../../Engine/ui/checkbox.h"
#include "../../../Engine/ui/FileInput.h"

class ObjectWindow
{
public:
	ObjectWindow::ObjectWindow(App* app)
	{
		m_app = app;
		m_extraTextures.clear();
		int maxCountExtraTextures = 8;
		for (size_t i = 0; i < maxCountExtraTextures; i++) {
			m_extraTextures.push_back("");
		}

		resetUI();
	}

	void resetUI()
	{
		m_path = "data/models/bath.ds";
		m_texture = "data/textures/wall01.dds";

		m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		m_rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_isAlpha = false;
		m_isShadow = false;

		for (size_t i = 0; i < m_extraTextures.size(); i++) {
			m_extraTextures[i] = "";
		}
	}

	void initialize()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_app->m_uiManager->getById(1));

		m_Window = new Window;
		m_app->m_uiManager->add(m_Window);
		m_Window->Initialize(600, 610, Options::screen_width - 620, menuTop->m_height + 5);
		m_Window->addHeader("Object properties", Window::HEADER_BUTTON_CLOSE);
		m_Window->addBody();
		m_Window->hide();
		m_Window->addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
			m_app->unselectModel();
		});
		shift += m_Window->m_y + m_Window->getHeader()->m_height + 5;

		FileInput* objectPath = new FileInput;
		m_Window->addChild(objectPath);
		objectPath->initialize(580, 28, m_Window->m_x + 10, shift);
		objectPath->getDialog()->setPath(objectPath->getDialog()->getCurrentPath() + "data/models");
		objectPath->getDialog()->addDefaultModelsFilters();
		objectPath->setValueRefLink(&typeid(std::string), &m_path);
		objectPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			LightShaderClass* shader = new LightShaderClass;
			shader->Initialize(m_app->getGraphic()->getD3D()->GetDevice());
			shader->addLights({ m_app->m_light });

			if (!m_app->m_selectedModel) {
				ModelClass* model = new ModelClass;
				model->addShader(shader);
				model->Initialize(m_app->getGraphic()->getD3D(), &m_path[0], {m_texture});
				model->setId(m_app->m_modelManager->getNextId());
				model->addLights({ m_app->m_light });
				
				m_app->m_modelManager->Add(model);
				m_app->m_selectedModel = model;

				MapEntity::ObjectFormat format;
				format.model = model;
				format.parent = 0;

				format.id = model->getId();
				format.name = "Object " + std::to_string(format.id);
				format.type = MapEntity::ObjectTypes::MODEL;
				format.position = m_app->getGraphic()->getCamera()->GetPosition();
				format.position.y /= 2;
				m_position = format.position;
				format.scale = model->GetScale();
				format.rotation = model->getRotation();
				format.path = m_path;
				format.texture = m_texture;

				format.params.insert(std::pair<std::string, std::string>("alpha", "0"));
				format.params.insert(std::pair<std::string, std::string>("shadow", "0"));
					
				m_app->m_mapEntities->add(format);
			}
			else {
				std::vector<std::string> textures = m_app->m_selectedModel->GetTextureClass()->getTexturesPath();
				m_app->m_selectedModel->Shutdown();
				m_app->m_selectedModel->addShader(shader);
				m_app->m_selectedModel->Initialize(m_app->getGraphic()->getD3D(), &m_path[0], textures);
			}
			this->updateObjectModel();
		});
		shift += objectPath->m_height + 5;

		FileInput* objectTexture = new FileInput;
		m_Window->addChild(objectTexture);
		objectTexture->initialize(580, 28, m_Window->m_x + 10, shift);
		objectTexture->getDialog()->setPath(objectTexture->getDialog()->getCurrentPath() + "data/textures");
		objectTexture->getDialog()->addDefaultImageFilters();
		objectTexture->setValueRefLink(&typeid(std::string), &m_texture);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			if (m_app->m_selectedModel) {
				this->loadTexture();
				this->updateObjectModel();
			}
		});
		shift += objectTexture->m_height + 5;


		Label* m_WindowLabelPosition = new Label;
		m_Window->addChild(m_WindowLabelPosition);
		m_WindowLabelPosition->Initialize(50, 20);
		m_WindowLabelPosition->Add("Position", m_Window->m_x + 5, shift);
		shift += m_WindowLabelPosition->m_height + 1;

		Input* objectPositionX = new Input;
		m_Window->addChild(objectPositionX);
		objectPositionX->initialize(100, 28, m_Window->m_x + 10, shift);
		objectPositionX->setValueRefLink(&typeid(float), &m_position.x);
		objectPositionX->setFloatFormat("%.2f");
		objectPositionX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateObjectModel();
		});
		Input* objectPositionY = new Input;
		m_Window->addChild(objectPositionY);
		objectPositionY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectPositionY->setValueRefLink(&typeid(float), &m_position.y);
		objectPositionY->setFloatFormat("%.2f");
		objectPositionY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateObjectModel();
		});
		Input* objectPositionZ = new Input;
		m_Window->addChild(objectPositionZ);
		objectPositionZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectPositionZ->setValueRefLink(&typeid(float), &m_position.z);
		objectPositionZ->setFloatFormat("%.2f");
		objectPositionZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateObjectModel();
		});
		shift += objectPositionX->m_height + 5;


		Label* m_WindowLabelScale = new Label;
		m_Window->addChild(m_WindowLabelScale);
		m_WindowLabelScale->Initialize(50, 20);
		m_WindowLabelScale->Add("Scale", m_Window->m_x + 5, shift);
		shift += m_WindowLabelScale->m_height + 1;

		Input* objectScaleX = new Input;
		m_Window->addChild(objectScaleX);
		objectScaleX->initialize(100, 28, m_Window->m_x + 10, shift);
		objectScaleX->setValueRefLink(&typeid(float), &m_scale.x);
		objectScaleX->setFloatFormat("%.2f");
		objectScaleX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateObjectModel();
		});
		Input* objectScaleY = new Input;
		m_Window->addChild(objectScaleY);
		objectScaleY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectScaleY->setValueRefLink(&typeid(float), &m_scale.y);
		objectScaleY->setFloatFormat("%.2f");
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateObjectModel();
		});
		Input* objectScaleZ = new Input;
		m_Window->addChild(objectScaleZ);
		objectScaleZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectScaleZ->setValueRefLink(&typeid(float), &m_scale.z);
		objectScaleZ->setFloatFormat("%.2f");
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateObjectModel();
		});
		shift += objectScaleX->m_height + 5;


		Label* m_WindowLabelRotate = new Label;
		m_Window->addChild(m_WindowLabelRotate);
		m_WindowLabelRotate->Initialize(50, 20);
		m_WindowLabelRotate->Add("Rotation", m_Window->m_x + 5, shift);
		shift += m_WindowLabelRotate->m_height + 1;

		Input* objecRotateX = new Input;
		m_Window->addChild(objecRotateX);
		objecRotateX->initialize(100, 28, m_Window->m_x + 10, shift);
		objecRotateX->setValueRefLink(&typeid(float), &m_rotation.x);
		objecRotateX->setFloatFormat("%.0f");
		objecRotateX->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateObjectModel();
		});
		Input* objecRotateY = new Input;
		m_Window->addChild(objecRotateY);
		objecRotateY->initialize(100, 28, m_Window->m_x + 112, shift);
		objecRotateY->setValueRefLink(&typeid(float), &m_rotation.y);
		objecRotateY->setFloatFormat("%.0f");
		objecRotateY->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateObjectModel();
		});
		Input* objecRotateZ = new Input;
		m_Window->addChild(objecRotateZ);
		objecRotateZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objecRotateZ->setValueRefLink(&typeid(float), &m_rotation.z);
		objecRotateZ->setFloatFormat("%.0f");
		objecRotateZ->addEventHandler(AbstractGui::EventType::KEYBOARD_DOWN, [this] {
			this->updateObjectModel();
		});
		shift += objecRotateX->m_height + 5;


		Checkbox* alphaObject = new Checkbox;
		m_Window->addChild(alphaObject);
		alphaObject->initialize();
		alphaObject->Add("Is alpha", m_Window->m_x + 10, shift);
		alphaObject->setValueRefLink(&typeid(int), &m_isAlpha);
		alphaObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			this->updateObjectModel();
		});
		shift += alphaObject->m_height + 5;

		Checkbox* shadowObject = new Checkbox;
		m_Window->addChild(shadowObject);
		shadowObject->initialize();
		shadowObject->Add("Is shadow", m_Window->m_x + 10, shift);
		shadowObject->setValueRefLink(&typeid(int), &m_isShadow);
		shadowObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			this->updateObjectModel();
		});
		shift += shadowObject->m_height + 5;


		// extra textures
		FileInput* objectExtraTexture = 0;
		for (size_t i = 0; i < m_extraTextures.size(); i++) {
			FileInput* objectExtraTexture = new FileInput;
			m_Window->addChild(objectExtraTexture);
			objectExtraTexture->initialize(580, 28, m_Window->m_x + 10, shift);
			objectExtraTexture->getDialog()->setPath(objectExtraTexture->getDialog()->getCurrentPath() + "data/textures");
			objectExtraTexture->getDialog()->addDefaultImageFilters();
			objectExtraTexture->setValueRefLink(&typeid(std::string), &m_extraTextures[i]);
			objectExtraTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this, i] {
				this->loadTexture(i);
				this->updateObjectModel();

			});
			shift += objectExtraTexture->m_height + 5;
		}


		Button* cloneObject = new Button;
		m_Window->addChild(cloneObject);
		cloneObject->initialize();
		cloneObject->Add("Clone", m_Window->m_x + m_Window->m_width - 110, m_Window->m_y + m_Window->m_height - 30);
		cloneObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_Window->hide();
			MapEntity::ObjectFormat* entity = m_app->m_mapEntities->getEntity(m_app->m_selectedModel->getId());
			m_app->m_selectedModel->hideBBox();
			m_app->m_selectedModel = m_app->m_mapEntities->copyModel(*entity, m_app->m_modelManager);
			m_app->m_selectedModel->showBBox();
			updateUiFromModel();
		});

		Button* deleteObject = new Button;
		m_Window->addChild(deleteObject);
		deleteObject->initialize();
		deleteObject->Add("Delete", m_Window->m_x + m_Window->m_width - 220, m_Window->m_y + m_Window->m_height - 30);
		deleteObject->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
			m_Window->hide();
			m_app->m_mapEntities->remove(m_app->m_selectedModel->getId());
			m_app->m_modelManager->remove(m_app->m_selectedModel->getId());
			m_app->unselectModel();
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
		m_isShadow = m_app->m_selectedModel->isShadow();
		
		MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
		for (size_t i = 0; i < m_extraTextures.size(); i++) {
			m_extraTextures[i] = "";
			if (i < editorFormat->extraTextures.size()) {
				m_extraTextures[i] = editorFormat->extraTextures[i];
			}
		}

		m_Window->setTitle("Object properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();
	}

	void updateObjectModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		m_Window->setTitle("Object properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();

		m_app->m_selectedModel->SetScale(m_scale);
		m_app->m_selectedModel->SetPosition(m_position);
		m_app->m_selectedModel->SetRotation(m_rotation);
		m_app->m_selectedModel->setAlpha(m_isAlpha);
		m_app->m_selectedModel->setShadow(m_isShadow);
		m_app->m_selectedModel->refreshBBox();

		MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
		editorFormat->path = m_path;
		editorFormat->texture = m_texture;
		editorFormat->position = m_position;
		editorFormat->scale = m_scale;
		editorFormat->rotation = m_rotation;
		editorFormat->params["alpha"] = std::to_string(m_isAlpha ? 1 : 0);
		editorFormat->params["shadow"] = std::to_string(m_isShadow ? 1 : 0);
		editorFormat->extraTextures = m_extraTextures;
	}

	void loadTexture()
	{
		m_app->m_selectedModel->ReleaseTexture();
		m_app->m_selectedModel->LoadTextures(m_texture);
	}

	void loadTexture(int index)
	{
		if (m_app->m_selectedModel->getSubset()) {
			ModelClass* model = dynamic_cast<ModelClass*>(m_app->m_selectedModel->getSubset()->getByIndex(index));
			if (model) {
				model->GetTextureClass()->ReleaseTexture(index);
				model->GetTextureClass()->setTexture(model->getD3D()->GetDevice(), m_extraTextures[index], index);
				return;
			}
		}

		m_extraTextures[index] = "";
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
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_scale;
	D3DXVECTOR3 m_rotation;
	int m_isAlpha;
	int m_isShadow;
	std::vector<std::string> m_extraTextures;
};