#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include <vector>
using namespace std;

#include "AbstractModel.h"
#include "../d3dclass.h"
#include "../cameraclass.h"
#include "../frustumclass.h"

class ModelManager
{
public:
    ModelManager();
    bool Initialize(D3DClass*);
    bool Add(AbstractModel* model);
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

private:
    std::vector<AbstractModel *> m_models;

    D3DClass* m_D3D;
    int m_RenderCount;
    int m_TriangleCount;
};

#endif