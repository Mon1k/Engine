#include "ModelManager.h"
#include "../Options.h"
#include "AbstractTarget.h"
#include "terrain/terrainclass.h"

ModelManager::ModelManager()
{
    m_models.clear();
    m_D3D = 0;
    m_DepthShader = 0;
    m_ShadowShader = 0;
    m_RenderStencilTexture = 0;
}

bool ModelManager::Initialize(D3DClass* d3d, FrustumClass* frustum)
{
    m_D3D = d3d;
    m_frustum = frustum;

    m_DepthShader = new DepthShaderClass;
    if (!m_DepthShader->Initialize(m_D3D->GetDevice())) {
        return false;
    }

    m_ShadowShader = new ShadowShaderClass;
    if (!m_ShadowShader->Initialize(m_D3D->GetDevice())) {
        return false;
    }

    m_RenderStencilTexture = new RenderStencilTextureClass;
    if (!m_RenderStencilTexture->InitializeFull(m_D3D->GetDevice(), Options::shadow_width, Options::shadow_height, Options::shadow_depth, Options::shadow_near)) {
        return false;
    }

    m_volumetricClouds = new VolumetricClouds;
    m_volumetricClouds->setD3D(m_D3D);
    if (!m_volumetricClouds->Initialize(m_D3D->GetDevice())) {
        return false;
    }

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

    if (m_RenderStencilTexture) {
        m_RenderStencilTexture->Shutdown();
        delete m_RenderStencilTexture;
        m_RenderStencilTexture = 0;
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
    }


}

void ModelManager::RenderShadowDepth(CameraClass* camera)
{
    size_t size = m_modelsShadow.size();

    D3DXMATRIX lightViewMatrix, lightProjectionMatrix, viewMatrix, projectionMatrix;

    viewMatrix = camera->getViewMatrix();
    m_D3D->GetProjectionMatrix(projectionMatrix);

    m_RenderStencilTexture->SetRenderTarget(m_D3D->GetDeviceContext());
    m_RenderStencilTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);
    

    LightClass* light;
    for (int i = 0; i < size; i++) {
        ModelClass* model = dynamic_cast<ModelClass*> (m_modelsShadow[i]);
        // @todo - later calculation for per light
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
}

void ModelManager::Render(CameraClass* camera)
{
    std::vector<AbstractModel*> modelsAlpha;
    D3DXMATRIX viewMatrix, projectionMatrix, orthoMatrix, worldMatrix, baseViewMatrix, lightViewMatrix, lightProjectionMatrix;

    //// clouds pass later queues pass from rendering
    m_volumetricClouds->computeShaders();

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
                if (Options::shadow_enabled && model && model->getLights().size() > 0 && m_modelsShadow.size() > 0) {
                    if (dynamic_cast<const TerrainClass*>(model) != nullptr) {
                        TerrainClass* terrain = dynamic_cast<TerrainClass*>(model);
                        terrain->Render(camera, m_RenderStencilTexture->GetShaderResourceView());
                    }
                    else if (dynamic_cast<const AbstractTarget*>(model) != nullptr) {
                        model->Render(camera);
                    }
                    else {
                        LightClass* light = model->getLight(0);
                        light->GenerateViewMatrix();

                        light->GetViewMatrix(lightViewMatrix);
                        light->GetProjectionMatrix(lightProjectionMatrix);

                        if (m_modelsRender[i]->getAlpha()) {
                            m_D3D->TurnOnAlphaBlending();
                        }

                        model->Render();
                        m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, model->GetTexture(), m_RenderStencilTexture->GetShaderResourceView(), light);

                        CompositeModel* subset = model->getSubset();
                        if (subset) {
                            for (int j = 0; j < subset->getChilds().size(); j++) {
                                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                                modelSubset->Render();
                                m_ShadowShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, modelSubset->GetTexture(), m_RenderStencilTexture->GetShaderResourceView(), light);
                            }
                        }

                        if (m_modelsRender[i]->getAlpha()) {
                            m_D3D->TurnOffAlphaBlending();
                        }
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