#ifndef _ABSTRACTMODEL_H_
#define _ABSTRACTMODEL_H_

#include "../AbstractNode.h"
#include "../d3dclass.h"
#include "../shaders/AbstractShader.h"
#include "../cameraclass.h"

class AbstractModel : public AbstractNode
{
public:
    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
    };
    ModelType* m_model;

public:
    AbstractModel()
    {
        m_vertexCount = 0;
        m_indexCount = 0;
    }

    virtual void Render(CameraClass* = 0) = 0;
    virtual void PreRender(CameraClass*)
    {
    };
    virtual void Shutdown() = 0;
    virtual void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&)
    {
    };

    void setVertexCount(int vertexCount) {
        m_vertexCount = vertexCount;
        m_model = new ModelType[m_vertexCount];
    }

    void setIndexCount(int indexCount) {
        m_indexCount = indexCount;
    }

    int GetIndexCount() {
        return m_indexCount;
    }

    int GetTtriangleCount() {
        return m_vertexCount / 3;
    }
    bool isVisible() {
        return visible;
    }
    void addShader(AbstractShader* shader)
    {
        m_shader = shader;
        m_shader->setD3D(m_D3D);
    }
    AbstractShader* getShader()
    {
        return m_shader;
    }

protected:
    int m_vertexCount, m_indexCount;
    bool visible = true;

    D3DClass* m_D3D;
    AbstractShader* m_shader;
};

#endif