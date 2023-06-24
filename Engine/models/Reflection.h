#ifndef _REFLECTION_H_
#define _REFLECTION_H_

#include "../modelclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/reflectionshaderclass.h"
#include "AbstractTarget.h"

class Reflection: public ModelClass, public AbstractTarget
{
public:
	Reflection();

	virtual bool Initialize(D3DClass*, char*, std::vector<std::string>);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	virtual void RenderToTexture(CameraClass*);
	virtual void Shutdown();

protected:
	RenderTextureClass* m_ReflectionTexture;
	D3DXMATRIX m_ReflectionMatrix;
};

#endif