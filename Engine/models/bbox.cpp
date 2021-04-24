#include "bbox.h"

BBox::BBox()
{
    color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
}

void BBox::CreateBox(ID3D11Device* device, HWND hwnd, D3DXVECTOR3 position, D3DXVECTOR3 size)
{
    m_ModelColor[0] = new ModelColorClass;
    m_ModelColor[0]->SetPoint(D3DXVECTOR3(position.x - size.x, position.y - size.y, position.z - size.z), D3DXVECTOR3(position.x + size.x, position.y - size.y, position.z - size.z));
    m_ModelColor[0]->SetColor(color);
    m_ModelColor[0]->Initialize(device);

    m_ModelColor[1] = new ModelColorClass;
    m_ModelColor[1]->SetPoint(D3DXVECTOR3(position.x - size.x, position.y - size.y, position.z - size.z), D3DXVECTOR3(position.x - size.x, position.y + size.y, position.z - size.z));
    m_ModelColor[1]->SetColor(color);
    m_ModelColor[1]->Initialize(device);

    m_ModelColor[2] = new ModelColorClass;
    m_ModelColor[2]->SetPoint(D3DXVECTOR3(position.x + size.x, position.y + size.y, position.z - size.z), D3DXVECTOR3(position.x + size.x, position.y - size.y, position.z - size.z));
    m_ModelColor[2]->SetColor(color);
    m_ModelColor[2]->Initialize(device);

    m_ModelColor[3] = new ModelColorClass;
    m_ModelColor[3]->SetPoint(D3DXVECTOR3(position.x + size.x, position.y + size.y, position.z - size.z), D3DXVECTOR3(position.x - size.x, position.y + size.y, position.z - size.z));
    m_ModelColor[3]->SetColor(color);
    m_ModelColor[3]->Initialize(device);


    m_ModelColor[4] = new ModelColorClass;
    m_ModelColor[4]->SetPoint(D3DXVECTOR3(position.x + size.x, position.y + size.y, position.z + size.z), D3DXVECTOR3(position.x + size.x, position.y - size.y, position.z + size.z));
    m_ModelColor[4]->SetColor(color);
    m_ModelColor[4]->Initialize(device);

    m_ModelColor[5] = new ModelColorClass;
    m_ModelColor[5]->SetPoint(D3DXVECTOR3(position.x + size.x, position.y + size.y, position.z + size.z), D3DXVECTOR3(position.x - size.x, position.y + size.y, position.z + size.z));
    m_ModelColor[5]->SetColor(color);
    m_ModelColor[5]->Initialize(device);
    
    m_ModelColor[6] = new ModelColorClass;
    m_ModelColor[6]->SetPoint(D3DXVECTOR3(position.x - size.x, position.y - size.y, position.z + size.z), D3DXVECTOR3(position.x + size.x, position.y - size.y, position.z + size.z));
    m_ModelColor[6]->SetColor(color);
    m_ModelColor[6]->Initialize(device);

    m_ModelColor[7] = new ModelColorClass;
    m_ModelColor[7]->SetPoint(D3DXVECTOR3(position.x - size.x, position.y - size.y, position.z + size.z), D3DXVECTOR3(position.x - size.x, position.y + size.y, position.z + size.z));
    m_ModelColor[7]->SetColor(color);
    m_ModelColor[7]->Initialize(device);


    m_ModelColor[8] = new ModelColorClass;
    m_ModelColor[8]->SetPoint(D3DXVECTOR3(position.x - size.x, position.y - size.y, position.z - size.z), D3DXVECTOR3(position.x - size.x, position.y - size.y, position.z + size.z));
    m_ModelColor[8]->SetColor(color);
    m_ModelColor[8]->Initialize(device);

    m_ModelColor[9] = new ModelColorClass;
    m_ModelColor[9]->SetPoint(D3DXVECTOR3(position.x + size.x, position.y - size.y, position.z - size.z), D3DXVECTOR3(position.x + size.x, position.y - size.y, position.z + size.z));
    m_ModelColor[9]->SetColor(color);
    m_ModelColor[9]->Initialize(device);

    m_ModelColor[10] = new ModelColorClass;
    m_ModelColor[10]->SetPoint(D3DXVECTOR3(position.x - size.x, position.y + size.y, position.z - size.z), D3DXVECTOR3(position.x - size.x, position.y + size.y, position.z + size.z));
    m_ModelColor[10]->SetColor(color);
    m_ModelColor[10]->Initialize(device);

    m_ModelColor[11] = new ModelColorClass;
    m_ModelColor[11]->SetPoint(D3DXVECTOR3(position.x + size.x, position.y + size.y, position.z - size.z), D3DXVECTOR3(position.x + size.x, position.y + size.y, position.z + size.z));
    m_ModelColor[11]->SetColor(color);
    m_ModelColor[11]->Initialize(device);


    m_ColorShader = new ColorShaderClass;
    m_ColorShader->Initialize(device, hwnd);
}

void BBox::Shutdown()
{
    for (int i = 0; i < 12; i++) {
        if (m_ModelColor[i]) {
            m_ModelColor[i]->Shutdown();
            delete m_ModelColor[i];
            m_ModelColor[i] = 0;
        }
    }

    if (m_ColorShader) {
        m_ColorShader->Shutdown();
        delete m_ColorShader;
        m_ColorShader = 0;
    }
}

void BBox::Render(D3DClass* m_D3D, D3DXMATRIX viewMatrix)
{
    D3DXMATRIX worldMatrix, projectionMatrix;

    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    for (int i = 0; i < 12; i++) {
        m_ModelColor[i]->Render(m_D3D->GetDeviceContext());
        m_ColorShader->Render(m_D3D->GetDeviceContext(), 2, worldMatrix, viewMatrix, projectionMatrix);
    }
}