#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

#include "AbstractModel.h"
#include "../d3dclass.h"
#include "../cameraclass.h"
#include "../frustumclass.h"

#include "../textures/depthshaderclass.h"
#include "../textures/shadowshaderclass.h"
#include "../lightclass.h"
#include "../render/rendertextureclass.h"

class ModelManager
{
public:
    ModelManager();
    bool Initialize(D3DClass*);
    bool Add(AbstractModel* model);
    void PreRender(CameraClass*, FrustumClass*);
    void RenderShadowDepth(std::vector<AbstractModel*>);
    void Render(CameraClass*, FrustumClass*);
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

private:
    std::vector<AbstractModel *> m_models;

    D3DClass* m_D3D;
    int m_RenderCount;
    int m_TriangleCount;

    DepthShaderClass* m_DepthShader;
    RenderTextureClass* m_RenderTexture;
};

#endif