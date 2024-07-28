#pragma once

#include <cmath>
#include <vector>
#include <algorithm>

#include "AbstractParticle.h"
#include "../../d3dclass.h"
#include "../../cameraclass.h"
#include "../../frustumclass.h"

class ParticleManager
{
public:
    ParticleManager();
    bool Initialize(D3DClass*);
    bool Add(AbstractParticle* model);
    void Render(CameraClass*, FrustumClass*);
    void frame(float, FrustumClass*);
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
    std::vector<AbstractParticle*> m_models;

    D3DClass* m_D3D;
    int m_RenderCount;
    int m_TriangleCount;
};
