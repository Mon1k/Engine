#include "ModelManager.h"

ModelManager::ModelManager()
{
    models.clear();
    m_D3D = 0;
}

bool ModelManager::Initialize(D3DClass* d3d)
{
    m_D3D = d3d;

    return true;
}

bool ModelManager::Add(AbstractModel* model)
{
    models.push_back(model);

    return true;
}

void ModelManager::Shutdown()
{
    int size = models.size();
    for (int i = 0; i < size; i++) {
        models[i]->Shutdown();
    }
}

void ModelManager::Render()
{
    int size = models.size();
    for (int i = 0; i < size; i++) {
        if (models[i]->isVisible()) {
            models[i]->Render();
        }
    }
}
