#pragma once

#include "../../Engine/systemclass.h"
#include "../../Engine/map/MapEntity.h"

class MainWindow;
class ObjectWindow;

class CollisionDetection;

class App : public SystemClass
{
public:
	virtual bool init();

	void initDefaultObjects();
	void IninMenuTop();

	void loadUI();
	void loadScene();

	bool frame();
	void frameUI();
	void frameScene();

	MapEntity::ObjectFormat* getObjectEditor(int id);
	void unselectModel();

public:
	UIManager* m_uiManager;
	ModelManager* m_modelManager;
	float m_Counters[2]{};

	CollisionDetection* m_CollisionDetection;

	Model* m_selectedModel;
	MapEntity* m_mapEntities;

	LightClass* m_light;

	MainWindow* m_mainWindow;
	ObjectWindow* m_objectUI;
};
