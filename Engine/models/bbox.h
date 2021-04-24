#ifndef _BBOX_H_
#define _BBOX_H_


#include "../d3dclass.h"

#include "modelcolor.h"
#include "../colorshaderclass.h"

class BBox
{
public:
    BBox();
    void CreateBox(ID3D11Device* device, HWND hwnd, D3DXVECTOR3 position, D3DXVECTOR3 size);
    void Render(D3DClass*, D3DXMATRIX);
    void Shutdown();
    void SetColor(D3DXVECTOR4 color) {
        this->color = color;
    }

private:
    ModelColorClass* m_ModelColor[12];
    ColorShaderClass* m_ColorShader;
    D3DXVECTOR4 color;
};

#endif