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
    }
    virtual void Shutdown() = 0;
    virtual void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&)
    {
    }

    virtual void frame(CameraClass* camera, float time)
    {

    }

    void setVertexCount(int vertexCount) {
        m_vertexCount = vertexCount;
        m_model = new ModelType[m_vertexCount];
    }

    void setIndexCount(int indexCount) {
        m_indexCount = indexCount;
    }

    int getVertexCount() {
        return m_vertexCount;
    }

    int GetIndexCount() {
        return m_indexCount;
    }

    virtual int GetTriangleCount() {
        return m_vertexCount / 3;
    }

    void setVisible(bool visible) {
        m_visible = visible;
    }
    
    bool isVisible() {
        return m_visible;
    }
    
    virtual void addShader(AbstractShader* shader)
    {
        m_shader = shader;
        m_shader->setD3D(m_D3D);
    }
    
    virtual AbstractShader* getShader()
    {
        return m_shader;
    }

    D3DXVECTOR3 GetPosition() {
        return position;
    }

    D3DXVECTOR3 GetScale() {
        return scale;
    }

    D3DXVECTOR3 getRotation() {
        return m_rotation;
    }

    virtual void SetPosition(D3DXVECTOR3 _position)
    {
        D3DXVECTOR3 delta;
        delta.x = _position.x - position.x;
        delta.y = _position.y - position.y;
        delta.z = _position.z - position.z;

        m_Min.x += delta.x;
        m_Max.x += delta.x;
        
        m_Min.y += delta.y;
        m_Max.y += delta.y;

        m_Min.z += delta.z;
        m_Max.z += delta.z;

        position = _position;
    }

    virtual void SetScale(D3DXVECTOR3 _scale)
    {
        D3DXVECTOR3 delta;
        D3DXVECTOR3 size = getSize();
        D3DXVECTOR3 center = getCenter();

        if (scale.x != _scale.x) {
            delta.x = _scale.x / scale.x;
            m_Min.x = center.x - size.x / 2 * delta.x;
            m_Max.x = center.x + size.x / 2 * delta.x;
        }
        if (scale.y != _scale.y) {
            delta.y = _scale.y / scale.y;
            m_Min.y = center.y - size.y / 2 * delta.y;
            m_Max.y = center.y + size.y / 2 * delta.y;
        }
        if (scale.z != _scale.z) {
            delta.z = _scale.z / scale.z;
            m_Min.z = center.z - size.z / 2 * delta.z;
            m_Max.z = center.z + size.z / 2 * delta.z;
        }
        
        scale = _scale;
    }

    virtual void SetRotation(D3DXVECTOR3 _rotation)
    {
        m_rotation = _rotation;
    }

    D3DXVECTOR3 getMinPosition()
    {
        return m_Min;
    }

    D3DXVECTOR3 getMaxPosition()
    {
        return m_Max;
    }

    D3DXVECTOR3 getSize()
    {
        D3DXVECTOR3 size;
        size.x = m_Max.x - m_Min.x;
        size.y = m_Max.y - m_Min.y;
        size.z = m_Max.z - m_Min.z;

        return size;
    }

    D3DXVECTOR3 getCenter()
    {
        D3DXVECTOR3 center;

        center.x = (m_Max.x + m_Min.x) / 2;
        center.y = (m_Max.y + m_Min.y) / 2;
        center.z = (m_Max.z + m_Min.z) / 2;

        return center;
    }

    void setAlpha(bool alpha)
    {
        m_isAlpha = alpha;
    }

    bool getAlpha() {
        return m_isAlpha;
    }

    void setShadow(bool shadow) {
        m_isShadow = shadow;
    }

    bool isShadow() {
        return m_isShadow;
    }

    D3DClass* getD3D()
    {
        return m_D3D;
    }

    void setD3D(D3DClass* d3d)
    {
        m_D3D = d3d;
    }

protected:
    int m_vertexCount, m_indexCount;
    bool m_visible = true;
    bool m_isAlpha = false;
    bool m_isShadow = false;

    D3DXVECTOR3 position;
    D3DXVECTOR3 scale;
    D3DXVECTOR3 m_rotation;
    D3DXVECTOR3 m_Min, m_Max;

    D3DClass* m_D3D;
    AbstractShader* m_shader;
};

#endif