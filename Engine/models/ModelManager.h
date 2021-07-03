#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <vector>
using namespace std;

#include "AbstractModel.h"

class ModelManager
{
public:
    ModelManager();
    bool Add(AbstractModel* model);
    void Render();
    void Shutdown();

private:
    std::vector<AbstractModel *> models;
};

#endif