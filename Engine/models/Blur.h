#ifndef _BLUR_H_
#define _BLUR_H_

#include "../modelclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/horizontalblurshaderclass.h"
#include "../textureshaderclass.h"
#include "AbstractTarget.h"

class Blur : public ModelClass, public AbstractTarget
{
public:
	virtual bool Initialize(D3DClass*, char*);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	void RenderToTexture(CameraClass*);
	void RenderToTextureBlur(CameraClass*);
	void RenderToTextureBig(CameraClass*);
	void Shutdown();

protected:
	RenderTextureClass* m_RenderToTexture;
	RenderTextureClass* m_RenderToTextureBlur;
	RenderTextureClass* m_RenderToTextureBig;

	TextureShaderClass* m_TextureShaderClass;
	D3DXMATRIX m_WorldMatrix;
};

#endif