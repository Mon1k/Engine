#ifndef _ABSTRACTMODEL_H_
#define _ABSTRACTMODEL_H_

class AbstractModel
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
    virtual void Render() {};
    virtual void Shutdown() {};

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

protected:
    int m_vertexCount, m_indexCount;
};

#endif