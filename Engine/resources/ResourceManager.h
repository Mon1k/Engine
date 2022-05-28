#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include <vector>
using namespace std;

#include "AbstractResource.h"

class ResourceManager
{
public:
    ResourceManager();
    void Shutdown();
    bool Add(AbstractResource*);

    AbstractResource* getById(int id)
    {
        int size = m_resources.size();
        for (int i = 0; i < size; i++) {
            if (m_resources[i]->getId() == id) {
                return m_resources[i];
            }
        }

        return NULL;
    }

private:
	std::vector<AbstractResource*> m_resources;
};

#endif