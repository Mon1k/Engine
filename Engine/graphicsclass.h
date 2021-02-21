#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"

#include "textureshaderclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "bitmapclass.h"
#include "textclass.h"

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

	D3DClass* getD3D() {
		return m_D3D;
	}

private:
	bool Render(float);

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;

	TextureShaderClass* m_TextureShader;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	Button* m_Button;
	Button* m_Button2;

public:
	Label* m_Label;
	Cursor* m_Cursor;
};

#endif