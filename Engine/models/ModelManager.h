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

#include "../render/renderstenciltextureclass.h"
#include "../render/rendertextureclass.h"
#include "../textures/depthshaderclass.h"
#include "../textures/shadowshaderclass.h"
#include "../render/passes/VolumetricClouds.h"
#include "../models/sky/WeatherManager.h"

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
    void RenderShadowDepth(CameraClass*);
    
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

    VolumetricClouds* getCloud()
    {
        return m_volumetricClouds;
    }

    ID3D11ShaderResourceView* getResource3()
    {
        return m_RenderTexture3->GetShaderResourceView();
    }

private:
    std::vector<AbstractModel *> m_models;
    std::vector<AbstractModel *> m_modelsRender;

    D3DClass* m_D3D;
    FrustumClass* m_frustum;

    int m_RenderCount;
    int m_TriangleCount;

    DepthShaderClass* m_DepthShader;
    ShadowShaderClass* m_ShadowShader;
    RenderStencilTextureClass* m_RenderStencilTexture;

    VolumetricClouds* m_volumetricClouds;
    RenderStencilTextureClass* m_RenderStencilTexture2;
    RenderTextureClass* m_RenderTexture3;
    WeatherManager* m_WeatherManager;

    std::vector<AbstractModel *> m_modelsShadow;
};

#endif