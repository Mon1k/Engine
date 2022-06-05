#ifndef _GLASS_H_
#define _GLASS_H_

#include "../modelclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/glassshaderclass.h"

class Glass : public ModelClass
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
	void setRefractionScale(float scale) {
		m_refractionScale = scale;
	}

protected:
	RenderTextureClass* m_ReflectionTexture;
	GlassShaderClass* m_GlassShader;
	D3DXMATRIX m_ReflectionMatrix;
	std::vector<AbstractModel*> m_modelsTarget;

	float m_refractionScale;
};

#endif