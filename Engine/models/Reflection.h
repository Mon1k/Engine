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
	void addTarget(AbstractModel* target)
	{
		m_modelsTarget.push_back(target);
	}

private:
	RenderTextureClass* m_RenderTextureReflection;
	std::vector<AbstractModel*> m_modelsTarget;
};

#endif