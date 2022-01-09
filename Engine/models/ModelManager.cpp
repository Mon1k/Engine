#include "ModelManager.h"

ModelManager::ModelManager()
{
    m_models.clear();
    m_D3D = 0;
}

bool ModelManager::Initialize(D3DClass* d3d)
{
    m_D3D = d3d;

    return true;
}

bool ModelManager::Add(AbstractModel* model)
{
    m_models.push_back(model);

    return true;
}

void ModelManager::Shutdown()
{
    int size = m_models.size();
    for (int i = 0; i < size; i++) {
        m_models[i]->Shutdown();
    }
}

void ModelManager::Render(CameraClass* camera, FrustumClass* frustum)
{
    m_RenderCount = 0;
    m_TriangleCount = 0;

    int size = m_models.size();
    for (int i = 0; i < size; i++) {
        if (m_models[i]->isVisible()) {
            D3DXVECTOR3 position, size;
            m_models[i]->GetBoundingBox(position, size);
            if (frustum->CheckRectangle(position, size)) {
                m_models[i]->Render(camera);
                m_TriangleCount += m_models[i]->GetTtriangleCount();
                m_RenderCount++;
            }
        }
    }
}
