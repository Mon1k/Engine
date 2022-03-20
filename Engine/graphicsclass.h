#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


#include "d3dclass.h"
#include "cameraclass.h"
#include "ui/UIManager.h"
#include "models/ModelManager.h"

#include "modelclass.h"

#include "render/rendertextureclass.h"
#include "render/2d/debugwindowclass.h"

#include "textures/refractionshaderclass.h"
#include "textures/watershaderclass.h"
#include "textures/reflectionshaderclass.h"
#include "textureshaderclass.h"
#include "lightshaderclass.h"

#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "frustumclass.h"
#include "timerclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	void frame(TimerClass*);
	bool Render();

	D3DClass* getD3D() {
		return m_D3D;
	};
	CameraClass* getCamera() {
		return m_Camera;
	};

	UIManager* getUiManager() {
		return m_uiManager;
	}
	ModelManager* getModelManager()
	{
		return m_modelManager;
	}

private:
	void RenderToTexture();
	void RenderRefractionToTextureWater();
	void RenderReflectionToTextureWater();
	void RenderScene();
	void RenderUI();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;

	UIManager* m_uiManager;
	ModelManager* m_modelManager;




	//////
	TextureShaderClass* m_TextureShader;

	// water
	ModelClass* m_GroundModel, * m_WallModel, * m_BathModel, * m_WaterModel;
	LightClass* m_LightWater;
	RenderTextureClass* m_RefractionTexture, * m_ReflectionTexture;
	LightShaderClass* m_LightShaderWater;
	RefractionShaderClass* m_RefractionShader;
	WaterShaderClass* m_WaterShader;
	float m_waterHeight, m_waterTranslation;

	// bitmap texture
	RenderTextureClass* m_RenderTexture;
	DebugWindowClass* m_DebugWindow;

	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	FrustumClass* m_Frustum;

	LightClass* m_Light1, * m_Light2;

	int m_RenderCount{};
	int m_TriangleCount{};
	float m_Counters[2]{};
};

#endif