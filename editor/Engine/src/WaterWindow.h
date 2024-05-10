#pragma once

#include "../main.h"

#include "../../../Engine/ui/UIManager.h"
#include "../../../Engine/ui/button.h"
#include "../../../Engine/ui/label.h"
#include "../../../Engine/ui/input.h"
#include "../../../Engine/ui/window.h"
#include "../../../Engine/ui/FileInput.h"

#include "../../../Engine/models/WaterNode.h"

class WaterWindow
{
public:
	WaterWindow::WaterWindow(App* app)
	{
		m_app = app;
		resetUI();
	}

	void resetUI()
	{
		m_path = "data/models/water.ds";
		m_texture = "data/textures/water01.dds";

		m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		m_height = 5;
		m_tiling = D3DXVECTOR2(0.05f, 0.05f);
		m_refractionScale = 0.02f;
		m_targetId = 0;
	}

	void initialize()
	{
		int shift = 0;
		Window* menuTop = dynamic_cast<Window*>(m_app->m_uiManager->getById(1));

		m_Window = new Window;
		m_app->m_uiManager->add(m_Window);
		m_Window->Initialize(600, 500, Options::screen_width - 620, menuTop->m_height + 5);
		m_Window->addHeader("Water properties", Window::HEADER_BUTTON_CLOSE);
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
				WaterNode* model = new WaterNode;
				model->Initialize(m_app->getGraphic()->getD3D(), &m_path[0], {m_texture});
				model->setId(m_app->m_modelManager->getNextId());
				model->addLights({ m_app->m_light });

				m_app->m_modelManager->Add(model);
				m_app->m_selectedModel = model;

				MapEntity::ObjectFormat format;
				format.model = model;
				format.parent = 0;

				format.id = model->getId();
				format.name = "Water " + std::to_string(format.id);
				format.type = MapEntity::ObjectTypes::WATER;
				format.position = model->GetPosition();
				format.scale = model->GetScale();
				format.rotation = model->getRotation();
				format.path = m_path;
				format.texture = m_texture;

				format.params.insert(std::pair<std::string, std::string>("height", std::to_string(m_height)));
				format.params.insert(std::pair<std::string, std::string>("tiling", std::to_string(m_tiling.x)+";"+ std::to_string(m_tiling.y)));
				format.params.insert(std::pair<std::string, std::string>("refraction_scale", std::to_string(m_refractionScale)));
				format.params.insert(std::pair<std::string, std::string>("targetId", std::to_string(m_targetId)));

				m_app->m_mapEntities->add(format);
			}
			this->updateWater();
		});
		shift += objectPath->m_height + 5;


		FileInput* objectTexture = new FileInput;
		m_Window->addChild(objectTexture);
		objectTexture->initialize(400, 28, m_Window->m_x + 10, shift);
		objectTexture->getDialog()->setPath(objectTexture->getDialog()->getCurrentPath() + "data/textures");
		objectTexture->getDialog()->addDefaultImageFilters();
		objectTexture->setValueRefLink(&typeid(std::string), &m_texture);
		objectTexture->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectTexture, this] {
			this->updateWater();
		});
		shift += objectTexture->m_height + 5;

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
			this->updateWater();
		});
		Input* objectPositionY = new Input;
		m_Window->addChild(objectPositionY);
		objectPositionY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectPositionY->setValueRefLink(&typeid(float), &m_position.y);
		objectPositionY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateWater();
		});
		Input* objectPositionZ = new Input;
		m_Window->addChild(objectPositionZ);
		objectPositionZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectPositionZ->setValueRefLink(&typeid(float), &m_position.z);
		objectPositionZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateWater();
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
			this->updateWater();
		});
		Input* objectScaleY = new Input;
		m_Window->addChild(objectScaleY);
		objectScaleY->initialize(100, 28, m_Window->m_x + 112, shift);
		objectScaleY->setValueRefLink(&typeid(float), &m_scale.y);
		objectScaleY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateWater();
		});
		Input* objectScaleZ = new Input;
		m_Window->addChild(objectScaleZ);
		objectScaleZ->initialize(100, 28, m_Window->m_x + 214, shift);
		objectScaleZ->setValueRefLink(&typeid(float), &m_scale.z);
		objectScaleZ->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateWater();
		});
		shift += objectScaleX->m_height + 5;


		Input* objectWaterHeight = new Input;
		m_Window->addChild(objectWaterHeight);
		objectWaterHeight->initialize(100, 28, m_Window->m_x + 10, shift);
		objectWaterHeight->setValueRefLink(&typeid(float), &m_height);
		objectWaterHeight->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterHeight, this] {
			this->updateWater();
		});
		shift += objectWaterHeight->m_height + 5;

		Input* objectWaterTilingX = new Input;
		m_Window->addChild(objectWaterTilingX);
		objectWaterTilingX->initialize(100, 28, m_Window->m_x + 10, shift);
		objectWaterTilingX->setValueRefLink(&typeid(float), &m_tiling.x);
		objectWaterTilingX->setFloatFormat("%.2f");
		objectWaterTilingX->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateWater();
		});
		Input* objectWaterTilingY = new Input;
		m_Window->addChild(objectWaterTilingY);
		objectWaterTilingY->initialize(100, 28, m_Window->m_x + objectWaterTilingX->m_width + 20, shift);
		objectWaterTilingY->setValueRefLink(&typeid(float), &m_tiling.y);
		objectWaterTilingY->setFloatFormat("%.2f");
		objectWaterTilingY->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterTilingY, this] {
			this->updateWater();
		});
		shift += objectWaterTilingX->m_height + 5;

		Input* objectWaterRefractionScale = new Input;
		m_Window->addChild(objectWaterRefractionScale);
		objectWaterRefractionScale->initialize(100, 28, m_Window->m_x + 10, shift);
		objectWaterRefractionScale->setValueRefLink(&typeid(float), &m_refractionScale);
		objectWaterRefractionScale->setFloatFormat("%.2f");
		objectWaterRefractionScale->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [objectWaterRefractionScale, this] {
			this->updateWater();
		});
		shift += objectWaterRefractionScale->m_height + 5;


		Input* objectWaterTarget = new Input;
		m_Window->addChild(objectWaterTarget);
		objectWaterTarget->initialize(100, 28, m_Window->m_x + 10, shift);
		objectWaterTarget->setValueRefLink(&typeid(int), &m_targetId);
		objectWaterTarget->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
			this->updateWater();
		});
		shift += objectWaterTarget->m_height + 5;
	}

	void updateUiFromModel()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		WaterNode* model = dynamic_cast<WaterNode*>(m_app->m_selectedModel);
		
		m_path = model->getPath();
		m_texture = model->GetTextureClass()->getTexturePath();
		m_position = model->GetPosition();
		m_scale = model->GetScale();
		m_height = model->getWaterHeight();
		m_tiling = model->getNormalMapTiling();
		m_refractionScale = model->getReflectRefractScale();
		if (model->getRefractionModel()) {
			m_targetId = model->getRefractionModel()->getId();
		}

		m_Window->setTitle("Water properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();
	}

	void updateWater()
	{
		if (!m_app->m_selectedModel) {
			return;
		}

		m_Window->setTitle("Water properties - " + std::to_string(m_app->m_selectedModel->getId()));
		m_Window->show();

		WaterNode* model = dynamic_cast<WaterNode*>(m_app->m_selectedModel);
		Model* modelWaterTarget = 0;

		model->Shutdown();
		model->Initialize(m_app->getGraphic()->getD3D(), &m_path[0], {m_texture});
		model->SetPosition(m_position);
		model->SetScale(m_scale);
		model->setWaterHeight(m_height);
		model->setNormalMapTiling(m_tiling.x, m_tiling.y);
		model->setReflectRefractScale(m_refractionScale);
		if (m_targetId) {
			Model* modelWaterTarget = dynamic_cast<Model*>(m_app->m_modelManager->getById(m_targetId));
			if (modelWaterTarget) {
				model->addRefractionTarget(modelWaterTarget);
			}
		}
		model->refreshBBox();

		MapEntity::ObjectFormat* editorFormat = m_app->getObjectEditor(m_app->m_selectedModel->getId());
		editorFormat->path = m_path;
		editorFormat->texture = m_texture;
		editorFormat->position = m_position;
		editorFormat->scale = m_scale;
		editorFormat->params["height"] = std::to_string(m_height);
		editorFormat->params["tiling"] = std::to_string(m_tiling.x) + ";" + std::to_string(m_tiling.y);
		editorFormat->params["refraction_scale"] = std::to_string(m_refractionScale);
		if (m_targetId && model->getRefractionModel()) {
			editorFormat->params["targetId"] = std::to_string(model->getRefractionModel()->getId());
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
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_scale;
	float m_height;
	D3DXVECTOR2 m_tiling;
	float m_refractionScale;
	int m_targetId;
};