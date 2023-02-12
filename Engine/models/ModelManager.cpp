#include "ModelManager.h"
#include "../Options.h"

ModelManager::ModelManager()
{
    m_models.clear();
    m_D3D = 0;
    m_DepthShader = 0;
    m_ShadowShader = 0;
    m_RenderTexture = 0;
}

bool ModelManager::Initialize(D3DClass* d3d)
{
    m_D3D = d3d;

    m_DepthShader = new DepthShaderClass;
    m_DepthShader->Initialize(m_D3D->GetDevice());

    m_ShadowShader = new ShadowShaderClass;
    m_ShadowShader->Initialize(m_D3D->GetDevice());

    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture->Initialize(m_D3D->GetDevice(), Options::shadow_width, Options::shadow_height)) {
        return false;
    }

    return true;
}

bool ModelManager::Add(AbstractModel* model)
{
    m_models.push_back(model);

    return true;
}

void ModelManager::Shutdown()
{
    int size = m_models.size();
    for (int i = 0; i < size; i++) {
        m_models[i]->Shutdown();
    }

    if (m_DepthShader) {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = 0;
    }

    if (m_ShadowShader) {
        m_ShadowShader->Shutdown();
        delete m_ShadowShader;
        m_ShadowShader = 0;
    }

    if (m_RenderTexture) {
        m_RenderTexture->Shutdown();
		delete m_RenderTexture;
        m_RenderTexture = 0;
	}
}

void ModelManager::PreRender(CameraClass* camera, FrustumClass* frustum)
{
    int size = m_models.size();

    m_modelsShadow.clear();

    for (int i = 0; i < size; i++) {
        if (m_models[i]->isVisible()) {
            if (m_models[i]->GetIndexCount() == 0) {
                m_models[i]->PreRender(camera);
            } else {
                D3DXVECTOR3 position, size;
                m_models[i]->GetBoundingBox(position, size);
                if (frustum->CheckRectangle(position, size)) {
                    m_models[i]->PreRender(camera);
                    if (m_models[i]->isShadow()) {
                        m_modelsShadow.push_back(m_models[i]);
                    }
                }
            }
        }
    }

    RenderShadowDepth();
}

void ModelManager::RenderShadowDepth()
{
    int size = m_modelsShadow.size();
    if (size < 1) {
        return;
    }

    D3DXMATRIX lightViewMatrix, lightProjectionMatrix, translateMatrix;

    // Set the render target to be the render to texture.
    m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

    // Clear the render to texture.
    m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);


    LightClass* light;
    for (int i = 0; i < size; i++) {
        ModelClass* model = dynamic_cast<ModelClass*> (m_modelsShadow[i]);
        light = model->getLight(0);
        light->GenerateViewMatrix();
        light->GetViewMatrix(lightViewMatrix);
        light->GetProjectionMatrix(lightProjectionMatrix);

        model->Render();
        m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix);
    }

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();
}

void ModelManager::Render(CameraClass* camera, FrustumClass* frustum)
{
    std::vector<AbstractModel*> modelsAlpha;
    D3DXMATRIX viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;

    modelsAlpha.clear();
    
    m_RenderCount = 0;
    m_TriangleCount = 0;
    
    int size = m_models.size();

    camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    for (int i = 0; i < size; i++) {
        if (m_models[i]->isVisible()) {
            if (m_models[i]->GetIndexCount() == 0) {
                m_models[i]->Render(camera);
            } else {
                D3DXVECTOR3 position, size;
                m_models[i]->GetBoundingBox(position, size);
                if (frustum->CheckRectangle(position, size)) {
                    if (m_models[i]->getAlpha()) {
                        modelsAlpha.push_back(m_models[i]);
                    } else {
                        if (m_models[i]->isShadow()) {
                            ModelClass* model = dynamic_cast<ModelClass*> (m_models[i]);
                            LightClass* light = model->getLight(0);
                            light->GenerateViewMatrix();
                            light->GetViewMatrix(lightViewMatrix);
                            light->GetProjectionMatrix(lightProjectionMatrix);

                            model->Render();
                            m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, model->GetTexture(), m_RenderTexture->GetShaderResourceView(), light->GetPosition(), light->GetAmbientColor(), light->GetDiffuseColor());
                        }
                        else {
                            m_models[i]->Render(camera);
                        }
                    }
                    m_TriangleCount += m_models[i]->GetTtriangleCount();
                    m_RenderCount++;
                }
            }
        }
    }


    size = modelsAlpha.size();
    // sort alpha model for render
    if (size > 0) {
        std::sort(begin(modelsAlpha), end(modelsAlpha), [&camera](AbstractModel* modelLeft, AbstractModel* modelRight) {
            float zCamera = camera->GetPosition().z;
            float zModelLeft = modelLeft->GetPosition().z;
            float zModelRight = modelRight->GetPosition().z;
            return abs(zCamera - zModelLeft) > abs(zCamera - zModelRight);
        });
        for (int i = 0; i < size; i++) {
            modelsAlpha[i]->Render(camera);
        }
    }
}
