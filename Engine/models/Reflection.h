#ifndef _REFLECTION_H_
#define _REFLECTION_H_

#include "../modelclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/reflectionshaderclass.h"

class Reflection: public ModelClass
{
public:
	virtual bool Initialize(D3DClass*, char*, std::vector<std::wstring>);
	virtual void PreRender(CameraClass*);
	virtual void Render(CameraClass*);
	void RenderToTexture(CameraClass*);
	void Shutdown();
	void addTarget(AbstractModel* target)
	{
		m_modelsTarget.push_back(target);
	}

protected:
	RenderTextureClass* m_ReflectionTexture;
	D3DXMATRIX m_ReflectionMatrix;
	std::vector<AbstractModel*> m_modelsTarget;
};

#endif