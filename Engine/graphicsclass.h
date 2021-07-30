#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "models/modelbumpclass.h"
#include "models/bbox.h"
#include "textures/multitextureshaderclass.h"

#include "render/rendertextureclass.h"
#include "render/2d/debugwindowclass.h"

#include "textures/specmapshaderclass.h"
#include "textures/bumpmapshaderclass.h"
#include "textures/lightmapshaderclass.h"
#include "textureshaderclass.h"
#include "alphamapshaderclass.h"
#include "lightshaderclass.h"
#include "fogshaderclass.h"

#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "modellistclass.h"
#include "frustumclass.h"

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
	bool Frame();
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

private:
	bool RenderToTexture();
	bool RenderScene();

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	
	ModelClass* m_Model;
	ModelClass* m_Model2;
	ModelBumpClass* m_Model3;
	ModelClass* m_ModelPlane;
	ModelClass* m_ModelPlane2;
	ModelClass* m_ModelPlane3;
	ModelClass* m_ModelPlane4;
	BBox* m_Bbox;
	
	SpecMapShaderClass* m_SpecMapShader;
	BumpMapShaderClass* m_BumpMapShader;
	TextureShaderClass* m_TextureShader;
	MultiTextureShaderClass* m_MultiTextureShader;
	LightMapShaderClass* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;
	FogShaderClass* m_FogShader;

	RenderTextureClass* m_RenderTexture;
	DebugWindowClass* m_DebugWindow;

	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;
	Label* m_Label2;

	int m_RenderCount;
	int m_TriangleCount;

public:
	Button* m_Button;
	Button* m_Button2;
	Label* m_Label;
	Checkbox* m_Checkbox;
	Cursor* m_Cursor;
};

#endif