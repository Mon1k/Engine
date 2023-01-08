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

void ModelManager::PreRender(CameraClass* camera, FrustumClass* frustum)
{
    int size = m_models.size();
    for (int i = 0; i < size; i++) {
        if (m_models[i]->isVisible()) {
            if (m_models[i]->GetIndexCount() == 0) {
                m_models[i]->PreRender(camera);
            } else {
                D3DXVECTOR3 position, size;
                m_models[i]->GetBoundingBox(position, size);
                if (frustum->CheckRectangle(position, size)) {
                    m_models[i]->PreRender(camera);
                }
            }
        }
    }
}

void ModelManager::Render(CameraClass* camera, FrustumClass* frustum)
{
    std::vector<AbstractModel*> modelsAlpha;
    m_RenderCount = 0;
    m_TriangleCount = 0;
    modelsAlpha.clear();

    int size = m_models.size();
    for (int i = 0; i < size; i++) {
        if (m_models[i]->isVisible()) {
            if (m_models[i]->GetIndexCount() == 0) {
                m_models[i]->Render(camera);
            } else {
                D3DXVECTOR3 position, size;
                m_models[i]->GetBoundingBox(position, size);
                if (frustum->CheckRectangle(position, size)) {
                    if (m_models[i]->getAlpha()) {
                        modelsAlpha.push_back(m_models[i]);
                    } else {
                        m_models[i]->Render(camera);
                    }
                    m_TriangleCount += m_models[i]->GetTtriangleCount();
                    m_RenderCount++;
                }
            }
        }
    }


    size = modelsAlpha.size();
    // sort alpha model for render
    if (size > 0) {
        std::sort(begin(modelsAlpha), end(modelsAlpha), [&camera](AbstractModel* modelLeft, AbstractModel* modelRight) {
            float zCamera = camera->GetPosition().z;
            float zModelLeft = modelLeft->GetPosition().z;
            float zModelRight = modelRight->GetPosition().z;
            return abs(zCamera - zModelLeft) > abs(zCamera - zModelRight);
        });
        for (int i = 0; i < size; i++) {
            modelsAlpha[i]->Render(camera);
        }
    }
}
