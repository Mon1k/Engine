#include "ModelManager.h"
#include "../Options.h"
#include "AbstractTarget.h"
#include "terrain/terrainclass.h"
#include "sky/skydomeclass.h"
#include "WaterNode.h"


#include "../ui/image.h"

ModelManager::ModelManager()
{
    m_models.clear();
    m_D3D = 0;
    m_DepthShader = 0;
    m_ShadowShader = 0;
    m_RenderStencilTexture.clear();
    m_lightShader = 0;

    m_volumetricClouds = 0;
    m_WeatherManager = 0;
    m_bitmapClouds = 0;

    m_lights.clear();
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

    m_lightShader = new LightShaderClass;
    m_lightShader->Initialize(m_D3D->GetDevice());

    /*m_volumetricClouds = new VolumetricClouds();
    m_volumetricClouds->setD3D(m_D3D);
    m_volumetricClouds->Initialize(m_D3D->GetDevice());
    
    m_WeatherManager = new WeatherManager(m_volumetricClouds);
    m_WeatherManager->setNextTarget();

    m_bitmapClouds = new BitmapClass();
    m_bitmapClouds->Initialize(m_D3D->GetDevice(), Options::screen_width, Options::screen_height, L"", Options::screen_width, Options::screen_height);*/

    return true;
}

bool ModelManager::Add(AbstractModel* model)
{
    m_models.push_back(model);

    return true;
}

bool ModelManager::addUnshift(AbstractModel* model)
{
    m_models.insert(m_models.begin(), model);

    return true;
}

void ModelManager::remove(int id)
{
    for (int i = 0; i < m_models.size(); i++) {
        if (m_models[i]->getId() == id) {
            m_models.erase(m_models.begin() + i);
            return;
        }
    }
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
    clearLights();

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

    if (m_volumetricClouds) {
        m_volumetricClouds->Shutdown();
        delete m_volumetricClouds;
        m_volumetricClouds = 0;
    }
    if (m_WeatherManager) {
        delete m_WeatherManager;
        m_WeatherManager = 0;
    }
    if (m_bitmapClouds) {
        m_bitmapClouds->Shutdown();
        delete m_bitmapClouds;
        m_bitmapClouds = 0;
    }

    if (m_lightShader) {
        m_lightShader->Shutdown();
        delete m_lightShader;
        m_lightShader = 0;
    }
}

void ModelManager::PreRender(CameraClass* camera)
{
    D3DXVECTOR3 position, size;

    m_modelsShadow.clear();
    m_modelsRender.clear();

    // add models for renderer
    for (size_t i = 0; i < m_models.size(); i++) {
        if (!m_models[i]->isVisible()) {
            continue;
        }

        if (m_models[i]->GetIndexCount() == 0) {
            m_modelsRender.push_back(m_models[i]);
        }
        else {
            m_models[i]->GetBoundingBox(position, size);
            // for shadow add width as height * 2, for render shadow behind normal bbox
            if (m_models[i]->isShadow()) {
                size.x += size.y * 2;
            }
            if (m_frustum->CheckRectangle(position, size)) {
                m_modelsRender.push_back(m_models[i]);
            }
        }
    }
    
    // add models for renderer shadow
    for (size_t i = 0; i < m_modelsRender.size(); i++) {
        // add models for reflections
        if (dynamic_cast<const AbstractTarget*>(m_modelsRender[i]) != nullptr) {
            AbstractTarget* targetScopes = dynamic_cast<AbstractTarget*>(m_modelsRender[i]);
            targetScopes->clearTargets();
            if (Options::reflectionLevel < 2) {
                for (size_t j = 0; j < m_modelsRender.size(); j++) {
                    if (i != j && dynamic_cast<const AbstractTarget*>(m_modelsRender[j]) == nullptr) {
                        if (Options::reflectionLevel == 1) {
                            if (dynamic_cast<const Model*>(m_modelsRender[j]) != nullptr) {
                                targetScopes->addTarget(m_modelsRender[j]);
                            }
                        }
                        else {
                            if (dynamic_cast<const SkyDomeClass*>(m_modelsRender[j]) != nullptr) {
                                continue;
                            }
                            targetScopes->addTarget(m_modelsRender[j]);
                        }
                    }
                }
            }
        }

        m_modelsRender[i]->PreRender(camera);
        if (Options::shadow_enabled && m_modelsRender[i]->isShadow()) {
            m_modelsShadow.push_back(m_modelsRender[i]);
        }
    }

    if (Options::shadow_enabled && m_modelsShadow.size() > 0) {
        RenderShadowDepth(camera);
    }
}

void ModelManager::RenderShadowDepth(CameraClass* camera)
{
    D3DXMATRIX lightProjectionMatrix, lightViewMatrix;
    std::vector<D3DXMATRIX> lightViewMatrixArray;

    for (size_t i = 0; i < m_lights.size(); i++) {
        if (i >= LightClass::NUM_LIGHTS) {
            break;
        }

        if (!m_lights[i]->isCastShadows()) {
            continue;
        }

        lightViewMatrixArray.clear();

        if (m_lights[i]->getType() == LightClass::LightType::LIGHT_POINT) {
            D3DXVECTOR3 look;
            D3DXVECTOR3 up;
            D3DXVECTOR3 position = m_lights[i]->GetPosition();

            // x+
            look = position + D3DXVECTOR3(1.0f, 0.0f, 0.0f);
            up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            D3DXMatrixLookAtLH(&lightViewMatrix, &position, &look, &up);
            lightViewMatrixArray.push_back(lightViewMatrix);

            // y+
            look = position + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
            D3DXMatrixLookAtLH(&lightViewMatrix, &position, &look, &up);
            lightViewMatrixArray.push_back(lightViewMatrix);

            // z+
            look = position + D3DXVECTOR3(0.0f, 0.0f, 1.0f);
            up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            D3DXMatrixLookAtLH(&lightViewMatrix, &position, &look, &up);
            lightViewMatrixArray.push_back(lightViewMatrix);

            // x-
            look = position + D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
            up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            D3DXMatrixLookAtLH(&lightViewMatrix, &position, &look, &up);
            lightViewMatrixArray.push_back(lightViewMatrix);

            // y-
            look = position + D3DXVECTOR3(0.0f, -1.0f, 0.0f);
            up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
            D3DXMatrixLookAtLH(&lightViewMatrix, &position, &look, &up);
            lightViewMatrixArray.push_back(lightViewMatrix);

            // z-
            look = position + D3DXVECTOR3(0.0f, 0.0f, -1.0f);
            up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
            D3DXMatrixLookAtLH(&lightViewMatrix, &position, &look, &up);
            lightViewMatrixArray.push_back(lightViewMatrix);
        }
        else {
            D3DXVECTOR3 dir = m_lights[i]->GetDirection();
            dir *= -1.0f * Options::shadow_width / 7;
            D3DXMatrixLookAtLH(&lightViewMatrix, &dir, &m_lights[i]->getLookAt(), &m_lights[i]->getUp());

            lightViewMatrixArray.push_back(lightViewMatrix);
        }


        m_RenderStencilTexture[i]->SetRenderTarget(m_D3D->GetDeviceContext());
        m_RenderStencilTexture[i]->ClearRenderTarget(m_D3D->GetDeviceContext());
        m_RenderStencilTexture[i]->GetOrthoMatrix(lightProjectionMatrix);

        for (size_t j = 0; j < m_modelsShadow.size(); j++) {
            ModelClass* model = dynamic_cast<ModelClass*>(m_modelsShadow[j]);

            model->Render();

            for (size_t k = 0; k < lightViewMatrixArray.size(); k++) {
                lightViewMatrix = lightViewMatrixArray[k];
                m_lights[i]->setViewMatrix(lightViewMatrix);

                m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, model->GetTexture());
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
    m_RenderCount = 0;
    m_TriangleCount = 0;

    if (!m_modelsRender.size()) {
        return;
    }

    std::vector<AbstractModel*> modelsAlpha;
    D3DXMATRIX viewMatrix, projectionMatrix, lightProjectionMatrix;

    modelsAlpha.clear();

    //m_lightShader->addLights(m_lights);
    std::vector<ID3D11ShaderResourceView*> shaderResources;
    shaderResources.clear();
    for (int j = 0; j < m_RenderStencilTexture.size(); j++) {
        shaderResources.push_back(m_RenderStencilTexture[j]->GetShaderResourceView());
    }

    camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    m_RenderStencilTexture[0]->GetOrthoMatrix(lightProjectionMatrix);

    for (size_t i = 0; i < m_modelsRender.size(); i++) {
        if (m_modelsRender[i]->GetIndexCount() == 0) {
            m_modelsRender[i]->Render(camera);
        } else {
            if (m_modelsRender[i]->getAlpha() && !m_modelsRender[i]->isShadow()) {
                modelsAlpha.push_back(m_modelsRender[i]);
            } else {
                ModelClass* model = dynamic_cast<ModelClass*> (m_modelsRender[i]);
                //model->addShader(m_lightShader);

                // @todo - its so ugly below
                if (model || dynamic_cast<const SkyDomeClass*>(model) != nullptr || dynamic_cast<const TerrainClass*>(model) != nullptr || dynamic_cast<const WaterNode*>(model) != nullptr) {
                    if (dynamic_cast<const TerrainClass*>(model) != nullptr) {
                        TerrainClass* terrain = dynamic_cast<TerrainClass*>(model);
                        terrain->Render(camera, m_modelsShadow.size() ? m_RenderStencilTexture[0]->GetShaderResourceView() : 0);
                    }
                    else if (dynamic_cast<const SkyDomeClass*>(model) != nullptr) {
                        model->Render(camera);

                        // render volumetric clouds
                        m_D3D->TurnOnAlphaFalseBlending();
                        m_bitmapClouds->Render(m_D3D->GetDeviceContext(), 0, 0);
                        m_volumetricClouds->Render(m_D3D->GetDeviceContext(), m_bitmapClouds->GetIndexCount());
                        m_D3D->TurnOffAlphaBlending();
                    }
                    else if (dynamic_cast<const AbstractTarget*>(model) != nullptr) {
                        model->Render(camera);
                    }
                    else if (dynamic_cast<const WaterNode*>(model) != nullptr) {
                        model->Render(camera);
                    }
                    else {
                        if (m_modelsRender[i]->getAlpha()) {
                            m_D3D->TurnOnAlphaBlending();
                        }

                        model->Render();
                        m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightProjectionMatrix, model->GetTexture(), shaderResources, m_lights);

                        CompositeModel* subset = model->getSubset();
                        if (subset) {
                            for (size_t j = 0; j < subset->getChilds().size(); j++) {
                                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                                modelSubset->Render();
                                m_ShadowShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), viewMatrix, projectionMatrix, lightProjectionMatrix, modelSubset->GetTexture(),shaderResources, m_lights);
                            }
                        }

                        if (m_modelsRender[i]->getAlpha()) {
                            m_D3D->TurnOffAlphaBlending();
                        }
                    }
                }
            }
        }

        m_TriangleCount += m_modelsRender[i]->GetTriangleCount();
        m_RenderCount++;
    }

    if (m_modelsShadow.size() > 0) {
        Image* image = new Image;
        image->m_baseViewMatrix = camera->getBaseViewMatrix();
        image->m_D3D = m_D3D;
        image->set3D(true);
        image->Initialize(Options::screen_width / 4, Options::screen_height / 4, 10, 80);
        image->loadTextureByResource(m_RenderStencilTexture[0]->GetShaderResourceView());
        image->Render();
    }


    size_t size = modelsAlpha.size();
    // sort alpha model for render
    if (size > 0) {
        std::sort(begin(modelsAlpha), end(modelsAlpha), [&camera](AbstractModel* modelLeft, AbstractModel* modelRight) {
            float zCamera = camera->GetPosition().z;
            float zModelLeft = modelLeft->GetPosition().z;
            float zModelRight = modelRight->GetPosition().z;
            return abs(zCamera - zModelLeft) > abs(zCamera - zModelRight);
        });
        for (size_t i = 0; i < size; i++) {
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

    //m_volumetricClouds->frame(camera, time);
    //m_WeatherManager->frame(time);
}

void ModelManager::addLights(std::vector<LightClass*> lights)
{
    for (int i = 0; i < lights.size(); i++) {
        addLight(lights[i]);
    }
}

void ModelManager::addLight(LightClass* light)
{
    int depthArray = 1;

    if (light->getType() == LightClass::LightType::LIGHT_POINT) {
        depthArray = 6;
    }

    this->m_lights.push_back(light);

    RenderStencilTextureClass* renderStencilTexture = new RenderStencilTextureClass;
    renderStencilTexture->Initialize(m_D3D->GetDevice(), Options::shadow_width, Options::shadow_height, Options::screen_near, Options::screen_depth, depthArray);
    m_RenderStencilTexture.push_back(renderStencilTexture);
}

void  ModelManager::removeLight(LightClass* light)
{
    for (int i = 0; i < m_lights.size(); i++) {
        if (m_lights[i] == light) {
            m_RenderStencilTexture[i]->Shutdown();

            m_lights.erase(m_lights.begin() + i);
            m_RenderStencilTexture.erase(m_RenderStencilTexture.begin() + i);

            break;
        }
    }
}

void ModelManager::clearLights()
{
    for (int i = 0; i < m_lights.size(); i++) {
        m_RenderStencilTexture[i]->Shutdown();
    }

    m_lights.clear();
    m_RenderStencilTexture.clear();
}