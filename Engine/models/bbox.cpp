#include "bbox.h"

BBox::BBox()
{
    color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
    m_shader = 0;

    for (int i = 0; i < 12; i++) {
        m_ModelColor[i] = 0;
    }
}

void BBox::CreateBox(D3DClass* d3d, D3DXVECTOR3 position, D3DXVECTOR3 size)
{
    ID3D11Device* device = d3d->GetDevice();
    m_D3D = d3d;

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


    ColorShaderClass* shader = new ColorShaderClass;
    shader->Initialize(device);
    addShader(shader);
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

    if (m_shader) {
        m_shader->Shutdown();
        m_shader = 0;
    }
}

void BBox::Render(CameraClass* camera)
{
    D3DXMATRIX worldMatrix, projectionMatrix, viewMatrix;

    camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    for (int i = 0; i < 12; i++) {
        m_ModelColor[i]->Render(m_D3D->GetDeviceContext());
        m_shader->Render(m_D3D->GetDeviceContext(), 2, worldMatrix, viewMatrix, projectionMatrix);
    }
}