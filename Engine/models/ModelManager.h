#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

#include "AbstractModel.h"
#include "CompositeModel.h"
#include "../d3dclass.h"
#include "../cameraclass.h"
#include "../frustumclass.h"

#include "Model.h"

#include "../render/rendertextureclass.h"
#include "../render/renderstenciltextureclass.h"
#include "../textures/depthshaderclass.h"
#include "../textures/shadowshaderclass.h"
#include "../textureshaderclass.h"
#include "../textures/blurshaderclass.h"
#include "../textures/softshadowshaderclass.h"
#include "../textures/shadowshader.h"
#include "2d/DebugWindow.h"

#include "../lightshaderclass.h"
#include "../lightclass.h"

class ModelManager
{
public:
    ModelManager();
    bool Initialize(D3DClass*, FrustumClass*);
    bool Add(AbstractModel* model);
    void PreRender(CameraClass*);
    void Render(CameraClass*);

    ////
    void RenderShadowDepth(CameraClass*);
    void RenderShadowShader(CameraClass*);
    void RenderBlurTexture(CameraClass*);
    void RenderBlur(CameraClass*);
    ID3D11ShaderResourceView* getRenderToTexture()
    {
        //return m_RenderTexture->GetShaderResourceView();
        return m_RenderStencilTexture->GetShaderResourceView();
    }
    ////
    
    
    void clear();
    void Shutdown();

    int getRenderCount()
    {
        return m_RenderCount;
    };
    int getTriangleCount()
    {
        return m_TriangleCount;
    };
    AbstractModel* getById(int id)
    {
        int size = m_models.size();
        for (int i = 0; i < size; i++) {
            if (m_models[i]->getId() == id) {
                return m_models[i];
            }
        }

        return NULL;
    }

    std::vector<AbstractModel*> getModels()
    {
        return m_models;
    }

    std::vector<AbstractModel*> getModelsRenderer()
    {
        return m_modelsRender;
    }

    int getNextId();
    void frame(CameraClass*, float);

    D3DClass* getD3D()
    {
        return m_D3D;
    }

    FrustumClass* getFrustum()
    {
        return m_frustum;
    }

private:
    std::vector<AbstractModel *> m_models;
    std::vector<AbstractModel *> m_modelsRender;

    D3DClass* m_D3D;
    FrustumClass* m_frustum;

    int m_RenderCount;
    int m_TriangleCount;

    //// for shadow and soft shadow - ugly
    ShadowShader* m_ShadowShader2;
    DepthShaderClass* m_DepthShader;
    ShadowShaderClass* m_ShadowShader;
    TextureShaderClass* m_TextureShader;
    BlurShaderClass* m_BlurShader;
    SoftShadowShaderClass* m_SoftShadowShader;
    DebugWindowClass* m_WindowTexture;
    
    RenderTextureClass* m_RenderTexture;
    RenderStencilTextureClass* m_RenderStencilTexture;
    RenderTextureClass* m_RenderTextureShadow;
    RenderTextureClass* m_RenderTextureBlurTexture;
    RenderTextureClass* m_RenderTextureBlur;
    std::vector<AbstractModel *> m_modelsShadow;


    ID3D11BlendState* g_pBlendStateNoBlend;
    ID3D11BlendState* g_pBlendStateColorWritesOff;
    ////
};

#endif