#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <vector>
using namespace std;

#include "AbstractModel.h"
#include "../d3dclass.h"

class ModelManager
{
public:
    ModelManager();
    bool Initialize(D3DClass*);
    bool Add(AbstractModel* model);
    void Render();
    void Shutdown();

private:
    std::vector<AbstractModel *> models;

    D3DClass* m_D3D;
};

#endif