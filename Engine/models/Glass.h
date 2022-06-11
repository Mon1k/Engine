#ifndef _GLASS_H_
#define _GLASS_H_

#include "../modelclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/glassshaderclass.h"
#include "AbstractTarget.h"

class Glass : public ModelClass, public AbstractTarget
{
public:
	virtual bool Initialize(D3DClass*, char*, std::vector<std::wstring>);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	void RenderToTexture(CameraClass*);
	void Shutdown();
	void setRefractionScale(float scale) {
		m_refractionScale = scale;
	}

protected:
	RenderTextureClass* m_ReflectionTexture;
	GlassShaderClass* m_GlassShader;
	D3DXMATRIX m_ReflectionMatrix;

	float m_refractionScale;
};

#endif