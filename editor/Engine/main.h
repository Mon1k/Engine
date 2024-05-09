#pragma once

#include "../../Engine/systemclass.h"
#include "../../Engine/map/MapEntity.h"

class MainWindow;
class ObjectWindow;
class TerrainWindow;
class WaterWindow;
class SkyWindow;

class CollisionDetection;

class App : public SystemClass
{
public:
	virtual bool init();

	void initDefaultObjects();
	void InitMenuTop();

	void loadUI();
	void loadScene();

	bool frame();
	void frameUI();
	void frameScene();

	MapEntity::ObjectFormat* getObjectEditor(int id);
	ObjectWindow* getObjectWindow();
	TerrainWindow* getTerrainWindow();
	WaterWindow* getWaterWindow();
	SkyWindow* getSkyWindow();
	void unselectModel();

public:
	UIManager* m_uiManager;
	ModelManager* m_modelManager;
	float m_Counters[2]{};

	CollisionDetection* m_CollisionDetection;

	Model* m_selectedModel;
	MapEntity* m_mapEntities;

	LightClass* m_light;

protected:
	MainWindow* m_mainWindow;
	ObjectWindow* m_objectWindow;
	TerrainWindow* m_terrainWindow;
	WaterWindow* m_waterWindow;
	SkyWindow* m_skyWindow;
};
