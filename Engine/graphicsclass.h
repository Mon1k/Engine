#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "models/bbox.h"
#include "textures/multitextureshaderclass.h"

#include "textures/lightmapshaderclass.h"
#include "textureshaderclass.h"
#include "alphamapshaderclass.h"
#include "lightshaderclass.h"

#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"
#include "modellistclass.h"
#include "frustumclass.h"

#include "ui/button.h"
#include "ui/label.h"
#include "ui/cursor.h"


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
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	
	ModelClass* m_Model;
	ModelClass* m_Model2;
	ModelClass* m_ModelPlane;
	ModelClass* m_ModelPlane2;
	ModelClass* m_ModelPlane3;
	BBox* m_Bbox;
	
	TextureShaderClass* m_TextureShader;
	MultiTextureShaderClass* m_MultiTextureShader;
	LightMapShaderClass* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;

	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	ModelListClass* m_ModelList;
	FrustumClass* m_Frustum;
	Label* m_Label2;

	int m_RenderCount;

public:
	Button* m_Button;
	Button* m_Button2;
	Label* m_Label;
	Cursor* m_Cursor;
};

#endif