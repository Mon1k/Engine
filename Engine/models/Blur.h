#ifndef _BLUR_H_
#define _BLUR_H_

#include "../modelclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/blurshaderclass.h"
#include "../textureshaderclass.h"
#include "AbstractTarget.h"

class Blur : public ModelClass, public AbstractTarget
{
public:
	virtual bool Initialize(D3DClass*, char*);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	void RenderToTexture(CameraClass*);
	void Shutdown();

protected:
	RenderTextureClass* m_RenderToTexture;

	BlurShaderClass* m_BlurShaderClass;
};

#endif