#ifndef _DEBUGWINDOW_H_
#define _DEBUGWINDOW_H_

#include "../../modelclass.h"
#include "../../render/rendertextureclass.h"
#include "../../render/2d/debugwindowclass.h"
#include "../../textureshaderclass.h"

class DebugWindow: public ModelClass
{
public:
	virtual bool Initialize(D3DClass*, int);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	void RenderToTexture(CameraClass*);
	void Shutdown();
	void addTarget(AbstractModel* target)
	{
		m_modelsTarget.push_back(target);
	}
	void setPosition(int x, int y)
	{
		m_x = x;
		m_y = y;
	}


protected:
	RenderTextureClass* m_RenderTexture;
	DebugWindowClass* m_DebugWindow;
	TextureShaderClass* m_TextureShader;
	std::vector<AbstractModel*> m_modelsTarget;

	int m_x, m_y;
};

#endif