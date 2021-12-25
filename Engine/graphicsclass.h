#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


#include "d3dclass.h"
#include "cameraclass.h"
#include "ui/UIManager.h"

#include "modelclass.h"
#include "models/modelbumpclass.h"
#include "models/bbox.h"
#include "textures/multitextureshaderclass.h"

#include "render/rendertextureclass.h"
#include "render/2d/debugwindowclass.h"


#include "textures/refractionshaderclass.h"
#include "textures/watershaderclass.h"
#include "textures/fadeshaderclass.h"
#include "textures/reflectionshaderclass.h"
#include "textures/transparentshaderclass.h"
#include "textures/translateshaderclass.h"
#include "textures/specmapshaderclass.h"
#include "textures/bumpmapshaderclass.h"
#include "textures/lightmapshaderclass.h"
#include "textureshaderclass.h"
#include "alphamapshaderclass.h"
#include "lightshaderclass.h"
#include "fogshaderclass.h"
#include "clipplaneshaderclass.h"

#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "modellistclass.h"
#include "frustumclass.h"
#include "timerclass.h"

#include "ui/button.h"
#include "ui/label.h"
#include "ui/cursor.h"
#include "ui/Checkbox.h"


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
	int GetRenderCount() {
		return m_RenderCount;
	};
	CameraClass* getCamera() {
		return m_Camera;
	};

	UIManager* getUiManager() {
		return m_uiManager;
	}

private:
	void RenderToTexture();
	void RenderToTextureReflection();
	void RenderToTextureFade();
	void RenderRefractionToTextureWater();
	void RenderReflectionToTextureWater();
	void RenderScene();
	void RenderUI();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;

	UIManager* m_uiManager;






	//////
	ModelClass* m_Model;
	ModelClass* m_Model2;
	ModelBumpClass* m_Model3;
	ModelClass* m_ModelPlane;
	ModelClass* m_ModelPlane2;
	ModelClass* m_ModelPlane3;
	ModelBumpClass* m_ModelPlane4;
	ModelClass* m_ModelPlane5;
	ModelClass* m_ModelPlane6;
	ModelClass* m_ModelPlane7;
	BBox* m_Bbox;
	
	TextureShaderClass* m_TextureShader;
	MultiTextureShaderClass* m_MultiTextureShader;
	LightMapShaderClass* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;
	BumpMapShaderClass* m_BumpMapShader;
	SpecMapShaderClass* m_SpecMapShader;
	FogShaderClass* m_FogShader;
	ClipPlaneShaderClass* m_ClipPlaneShader;
	TranslateShaderClass* m_TranslateShader;
	TransparentShaderClass* m_TransparentShader;
	ReflectionShaderClass* m_ReflectionShader;
	FadeShaderClass* m_FadeShader;

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

	// reflection
	RenderTextureClass* m_RenderTextureReflection;

	// fade
	RenderTextureClass* m_RenderTextureFade;
	BitmapClass* m_BitmapFade;
	float m_fadeInTime, m_accumulatedTime, m_fadePercentage;
	bool m_fadeDone;

	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;
	Label* m_Label2;

	LightClass* m_Light1, * m_Light2;

	int m_RenderCount{};
	int m_TriangleCount{};
	float m_Counters[2]{};

public:
	Button* m_Button;
	Button* m_Button2;
	Label* m_Label;
	Checkbox* m_Checkbox;
	Cursor* m_Cursor;
};

#endif