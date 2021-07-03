#include "ModelManager.h"

ModelManager::ModelManager()
{
    models.clear();
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
        models[i]->Render();
    }
}
