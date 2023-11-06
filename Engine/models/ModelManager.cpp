#include "ModelManager.h"
#include "../Options.h"
#include "AbstractTarget.h"

ModelManager::ModelManager()
{
    m_models.clear();
    m_D3D = 0;
    m_DepthShader = 0;
    m_ShadowShader = 0;
    m_SoftShadowShader = 0;
    m_BlurShader = 0;
    m_RenderTexture = 0;
    m_RenderTextureShadow = 0;
    m_RenderTextureBlur = 0;
}

bool ModelManager::Initialize(D3DClass* d3d, FrustumClass* frustum)
{
    m_D3D = d3d;
    m_frustum = frustum;

    m_ShadowShader2 = new ShadowShader;
    m_ShadowShader2->Initialize(m_D3D->GetDevice());

    m_DepthShader = new DepthShaderClass;
    m_DepthShader->Initialize(m_D3D->GetDevice());

    m_ShadowShader = new ShadowShaderClass;
    m_ShadowShader->Initialize(m_D3D->GetDevice());

    m_TextureShader = new TextureShaderClass;
    m_TextureShader->Initialize(m_D3D->GetDevice());

    m_BlurShader = new BlurShaderClass;
    m_BlurShader->Initialize(m_D3D->GetDevice());

    m_SoftShadowShader = new SoftShadowShaderClass;
    m_SoftShadowShader->Initialize(m_D3D->GetDevice());

    m_WindowTexture = new DebugWindowClass();
    m_WindowTexture->Initialize(m_D3D, m_D3D->getScreenWidth(), m_D3D->getScreenHeight(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight());

    m_RenderTexture = new RenderTextureClass;
    m_RenderTexture->InitializeFull(m_D3D->GetDevice(), Options::shadow_width, Options::shadow_height, Options::shadow_depth, Options::shadow_near);

    m_RenderTextureShadow = new RenderTextureClass;
    m_RenderTextureShadow->Initialize(m_D3D->GetDevice(), Options::shadow_width, Options::shadow_height);

    m_RenderTextureBlurTexture = new RenderTextureClass;
    m_RenderTextureBlurTexture->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight());

    m_RenderTextureBlur = new RenderTextureClass;
    m_RenderTextureBlur->Initialize(m_D3D->GetDevice(), m_D3D->getScreenWidth(), m_D3D->getScreenHeight());

    return true;
}

bool ModelManager::Add(AbstractModel* model)
{
    m_models.push_back(model);

    return true;
}

void ModelManager::clear()
{
    for (int i = 0; i < m_models.size(); i++) {
        m_models[i]->Shutdown();
    }
    m_models.clear();
}

void ModelManager::Shutdown()
{
    clear();

    if (m_ShadowShader2) {
        m_ShadowShader2->Shutdown();
        delete m_ShadowShader2;
        m_ShadowShader2 = 0;
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

    if (m_TextureShader) {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    if (m_BlurShader) {
        m_BlurShader->Shutdown();
        delete m_BlurShader;
        m_BlurShader = 0;
    }

    if (m_SoftShadowShader) {
        m_SoftShadowShader->Shutdown();
        delete m_SoftShadowShader;
        m_SoftShadowShader = 0;
    }

    if (m_WindowTexture) {
        m_WindowTexture->Shutdown();
        delete m_WindowTexture;
        m_WindowTexture = 0;
    }

    

    if (m_RenderTexture) {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = 0;
    }

    if (m_RenderTextureShadow) {
        m_RenderTextureShadow->Shutdown();
        delete m_RenderTextureShadow;
        m_RenderTextureShadow = 0;
    }

    if (m_RenderTextureBlurTexture) {
        m_RenderTextureBlurTexture->Shutdown();
        delete m_RenderTextureBlurTexture;
        m_RenderTextureBlurTexture = 0;
    }

    if (m_RenderTextureBlur) {
        m_RenderTextureBlur->Shutdown();
        delete m_RenderTextureBlur;
        m_RenderTextureBlur = 0;
    }
}

void ModelManager::PreRender(CameraClass* camera)
{
    D3DXVECTOR3 position, size;

    m_modelsShadow.clear();
    m_modelsRender.clear();

    for (int i = 0; i < m_models.size(); i++) {
        if (!m_models[i]->isVisible()) {
            continue;
        }

        if (m_models[i]->GetIndexCount() == 0) {
            m_modelsRender.push_back(m_models[i]);
        }
        else {
            m_models[i]->GetBoundingBox(position, size);
            if (m_frustum->CheckRectangle(position, size)) {
                m_modelsRender.push_back(m_models[i]);
            }
        }
    }

    for (int i = 0; i < m_modelsRender.size(); i++) {
        if (Options::reflectionLevel < 2 && dynamic_cast<const AbstractTarget*>(m_modelsRender[i]) != nullptr) {
            AbstractTarget* targetScopes = dynamic_cast<AbstractTarget*>(m_modelsRender[i]);
            targetScopes->clearTargets();
            for (int j = 0; j < m_modelsRender.size(); j++) {
                if (i != j && dynamic_cast<const AbstractTarget*>(m_modelsRender[j]) == nullptr) {
                    if (Options::reflectionLevel == 1) {
                        if (dynamic_cast<const Model*>(m_modelsRender[j]) != nullptr) {
                            targetScopes->addTarget(m_modelsRender[j]);
                        }
                    }
                    else {
                        targetScopes->addTarget(m_modelsRender[j]);
                    }
                }
            }
        }

        m_modelsRender[i]->PreRender(camera);
        if (Options::shadow_enabled && m_modelsRender[i]->isShadow()) {
            m_modelsShadow.push_back(m_modelsRender[i]);
        }
    }

    if (m_modelsShadow.size() > 0) {
        RenderShadowDepth(camera);
        if (Options::soft_shadow) {
            RenderShadowShader(camera);
            RenderBlurTexture(camera);
            RenderBlur(camera);
        }
    }
}

void ModelManager::RenderShadowDepth(CameraClass* camera)
{
    size_t size = m_modelsShadow.size();
    if (size < 1) {
        return;
    }

    D3DXMATRIX lightViewMatrix, lightProjectionMatrix, viewMatrix, projectionMatrix;


    viewMatrix = camera->getViewMatrix();
    m_D3D->GetProjectionMatrix(projectionMatrix);


    // Set the render target to be the render to texture.
    m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

    // Clear the render to texture.
    m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    LightClass* light;
    for (int i = 0; i < size; i++) {
        ModelClass* model = dynamic_cast<ModelClass*> (m_modelsShadow[i]);
        // @todo - later calculation for per light
        light = model->getLight(0);
        light->GenerateViewMatrix();
        
        light->GetViewMatrix(lightViewMatrix);
        light->GetProjectionMatrix(lightProjectionMatrix);
        //light->GetOrthoMatrix(lightProjectionMatrix);

        model->Render();
        /*m_ShadowShader2->SetShaderParameters(m_D3D->GetDeviceContext(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, light, NULL, NULL);
        m_ShadowShader2->RenderShaderDepth(m_D3D->GetDeviceContext(), model->GetIndexCount());*/
        m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, model->GetTexture());
    }

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();
}

/*void ModelManager::RenderShadowDepth(CameraClass* camera)
{
    int size = m_modelsShadow.size();
    if (size < 1) {
        return;
    }

    D3DXMATRIX lightViewMatrix, lightProjectionMatrix;

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
        m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, model->GetTexture());
        CompositeModel* subset = model->getSubset();
        if (subset) {
            for (int j = 0; j < subset->getChilds().size(); j++) {
                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                modelSubset->Render();
                m_DepthShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, modelSubset->GetTexture());
            }
        }
    }

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();
}*/

void ModelManager::RenderShadowShader(CameraClass* camera)
{
    int size = m_modelsShadow.size();
    if (size < 1) {
        return;
    }

    D3DXMATRIX lightViewMatrix, lightProjectionMatrix, viewMatrix, projectionMatrix;

    // Set the render target to be the render to texture.
    m_RenderTextureShadow->SetRenderTarget(m_D3D->GetDeviceContext());

    // Clear the render to texture.
    m_RenderTextureShadow->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    camera->Render();

    camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    LightClass* light;
    for (int i = 0; i < size; i++) {
        ModelClass* model = dynamic_cast<ModelClass*> (m_modelsShadow[i]);
        light = model->getLight(0);
        light->GenerateViewMatrix();
        light->GetViewMatrix(lightViewMatrix);
        light->GetProjectionMatrix(lightProjectionMatrix);

        model->Render();
        m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, model->GetTexture(), m_RenderTexture->GetShaderResourceView(), light);
        CompositeModel* subset = model->getSubset();
        if (subset) {
            for (int j = 0; j < subset->getChilds().size(); j++) {
                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                modelSubset->Render();
                m_ShadowShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, modelSubset->GetTexture(), m_RenderTexture->GetShaderResourceView(), light);
            }
        }
    }

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();
}

void ModelManager::RenderBlurTexture(CameraClass* camera)
{
    if (m_modelsShadow.size() < 1) {
        return;
    }

    D3DXMATRIX baseViewMatrix, orthoMatrix, worldMatrix;

    // Set the render target to be the render to texture.
    m_RenderTextureBlurTexture->SetRenderTarget(m_D3D->GetDeviceContext());

    // Clear the render to texture.
    m_RenderTextureBlurTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_D3D->GetWorldMatrix(worldMatrix);
    baseViewMatrix = camera->getBaseViewMatrix();
    m_RenderTextureBlurTexture->GetOrthoMatrix(orthoMatrix);

    m_D3D->TurnZBufferOff();

    m_WindowTexture->Render(0, 0);
    m_TextureShader->Render(m_D3D->GetDeviceContext(), m_WindowTexture->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
        m_RenderTextureShadow->GetShaderResourceView());

    m_D3D->TurnZBufferOn();

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();
}

void ModelManager::RenderBlur(CameraClass* camera)
{
    if (m_modelsShadow.size() < 1) {
        return;
    }

    D3DXMATRIX orthoMatrix, baseViewMatrix, worldMatrix;

    // Set the render target to be the render to texture.
    m_RenderTextureBlur->SetRenderTarget(m_D3D->GetDeviceContext());

    // Clear the render to texture.
    m_RenderTextureBlur->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    baseViewMatrix = camera->getBaseViewMatrix();
    m_D3D->GetWorldMatrix(worldMatrix);
    m_RenderTextureBlur->GetOrthoMatrix(orthoMatrix);

    m_D3D->TurnZBufferOff();

    m_WindowTexture->Render(0, 0);
    m_BlurShader->Render(m_D3D->GetDeviceContext(), m_WindowTexture->GetIndexCount(), worldMatrix, baseViewMatrix,
        orthoMatrix, m_RenderTextureBlurTexture->GetShaderResourceView(), m_D3D->getScreenWidth() / 4, m_D3D->getScreenHeight() / 4);

    m_D3D->TurnZBufferOn();

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();
}

void ModelManager::Render(CameraClass* camera)
{
    std::vector<AbstractModel*> modelsAlpha;
    D3DXMATRIX viewMatrix, projectionMatrix, orthoMatrix, worldMatrix, baseViewMatrix, lightViewMatrix, lightProjectionMatrix;

    modelsAlpha.clear();
    
    m_RenderCount = 0;
    m_TriangleCount = 0;
    
    
    camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    for (int i = 0; i < m_modelsRender.size(); i++) {
        if (m_modelsRender[i]->GetIndexCount() == 0) {
            m_modelsRender[i]->Render(camera);
        } else {
            if (m_modelsRender[i]->getAlpha() && !m_modelsRender[i]->isShadow()) {
                modelsAlpha.push_back(m_modelsRender[i]);
            } else {
                ModelClass* model = dynamic_cast<ModelClass*> (m_modelsRender[i]);
                if (Options::shadow_enabled && model && model->getLights().size() > 0/* && m_modelsRender[i]->isShadow()*/) {
                    
                    LightClass* light = model->getLight(0);
                    light->GenerateViewMatrix();

                    light->GetViewMatrix(lightViewMatrix);
                    light->GetProjectionMatrix(lightProjectionMatrix);
                    //light->GetOrthoMatrix(lightProjectionMatrix);
                    
                    if (m_modelsRender[i]->getAlpha()) {
                        m_D3D->TurnOnAlphaBlending();
                    }

                    model->Render();
                    if (Options::soft_shadow) {
                        m_SoftShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix,
                            model->GetTexture(), m_RenderTextureBlur->GetShaderResourceView(), light->GetPosition(), light->GetAmbientColor(), light->GetDiffuseColor());

                        CompositeModel* subset = model->getSubset();
                        if (subset) {
                            for (int j = 0; j < subset->getChilds().size(); j++) {
                                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                                modelSubset->Render();
                                m_SoftShadowShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), viewMatrix, projectionMatrix,
                                    modelSubset->GetTexture(), m_RenderTextureBlur->GetShaderResourceView(), light->GetPosition(), light->GetAmbientColor(), light->GetDiffuseColor());
                            }
                        }
                    }
                    else {
                        m_ShadowShader2->SetShaderParameters(m_D3D->GetDeviceContext(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, light, model->GetTexture(), m_RenderTexture->GetShaderResourceView());
                        m_ShadowShader2->RenderShader(m_D3D->GetDeviceContext(), model->GetIndexCount());
                        /*m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, model->GetTexture(), m_RenderTexture->GetShaderResourceView(), light);
                            
                        // @todo
                        CompositeModel* subset = model->getSubset();
                        if (subset) {
                            for (int j = 0; j < subset->getChilds().size(); j++) {
                                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                                modelSubset->Render();
                                m_ShadowShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, modelSubset->GetTexture(), m_RenderTexture->GetShaderResourceView(), light);
                            }
                        }*/
                    }

                    if (m_modelsRender[i]->getAlpha()) {
                        m_D3D->TurnOffAlphaBlending();
                    }
                }
                else {
                    m_modelsRender[i]->Render(camera);
                }
            }
        }

        m_TriangleCount += m_modelsRender[i]->GetTriangleCount();
        m_RenderCount++;
    }


    int size = modelsAlpha.size();
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

int ModelManager::getNextId()
{
    int nextId = 0;
    int modelId;
    for (int i = 0; i < m_models.size(); i++) {
        modelId = m_models[i]->getId();
        if (nextId < modelId) {
            nextId = modelId;
        }
    }
    nextId++;

    return nextId;
}


void ModelManager::frame(CameraClass* camera, float time)
{
    for (int i = 0; i < m_modelsRender.size(); i++) {
        m_modelsRender[i]->frame(camera, time);
    }
}