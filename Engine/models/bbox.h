#ifndef _BBOX_H_
#define _BBOX_H_

#include "../d3dclass.h"

#include "AbstractModel.h"
#include "modelcolor.h"
#include "../colorshaderclass.h"
#include "../cameraclass.h"

class BBox: public AbstractModel
{
public:
    BBox();
    void CreateBox(D3DClass* d3d, D3DXVECTOR3 position, D3DXVECTOR3 size);
    void fillValues(D3DXVECTOR3 position, D3DXVECTOR3 size);
    virtual void Render(CameraClass*);
    virtual void Render();
    virtual void Shutdown();
    
    void SetColor(D3DXVECTOR4 color)
    {
        this->color = color;
    }

    void setCamera(CameraClass* camera)
    {
        m_camera = camera;
    }

    void reCreate(D3DXVECTOR3 position, D3DXVECTOR3 size);

private:
    ModelColorClass* m_ModelColor[12];
    D3DXVECTOR4 color;

    CameraClass* m_camera;
};

#endif