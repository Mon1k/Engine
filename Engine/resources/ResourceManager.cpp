#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	m_resources.clear();
}

bool ResourceManager::Add(AbstractResource* model)
{
    m_resources.push_back(model);

    return true;
}

void ResourceManager::Shutdown()
{
    size_t size = m_resources.size();
    for (size_t i = 0; i < size; i++) {
        m_resources[i]->Shutdown();
    }
}