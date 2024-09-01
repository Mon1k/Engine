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
    m_RenderStencilTexture = 0;

    m_volumetricClouds = 0;
    m_WeatherManager = 0;
    m_bitmapClouds = 0;
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
    if (!m_RenderStencilTexture->InitializeFull(m_D3D->GetDevice(), Options::shadow_width, Options::shadow_height, 1000.0f, 0.1f)) {
        return false;
    }

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
    size_t size = m_modelsShadow.size();

    D3DXMATRIX lightViewMatrix, lightProjectionMatrix, viewMatrix, projectionMatrix;

    viewMatrix = camera->getViewMatrix();
    m_D3D->GetProjectionMatrix(projectionMatrix);

    m_RenderStencilTexture->SetRenderTarget(m_D3D->GetDeviceContext());
    m_RenderStencilTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 0.0f);
    

    LightClass* light;
    for (size_t i = 0; i < size; i++) {
        ModelClass* model = dynamic_cast<ModelClass*> (m_modelsShadow[i]);
        // @todo - later calculation for per light
        light = model->getLight(0);
        if (!light->isCastShadows()) {
            continue;
        }


        // set cascades
        int NumCascades = 4;
        float CascadeSplits[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        float MinDistance = 0.0000f;
        float MaxDistance = 1.0000f;

        CascadeSplits[0] = MinDistance + 0.0500f * MaxDistance;
        CascadeSplits[1] = MinDistance + 0.1500f * MaxDistance;
        CascadeSplits[2] = MinDistance + 0.5000f * MaxDistance;
        CascadeSplits[3] = MinDistance + 1.0000f * MaxDistance;


        // calc matrixes
        light->GetViewMatrix(lightViewMatrix);
        //light->GetProjectionMatrix(lightProjectionMatrix);
        light->GetOrthoMatrix(lightProjectionMatrix);

        D3DXMATRIX invViewProj, invView;
        light->GetProjectionMatrix(invViewProj);
        //m_D3D->GetProjectionMatrix(invViewProj);
        lightViewMatrix = camera->getWorldMatrix();

        //D3DXMatrixInverse(&lightViewMatrix, NULL, &lightViewMatrix);
        D3DXMatrixInverse(&invViewProj, NULL, &invViewProj);
        invViewProj = invViewProj * lightViewMatrix;

        D3DXVECTOR3 frustumCorners[8] = {
            D3DXVECTOR3(-1.0f,  1.0f, 0.0f),
            D3DXVECTOR3(1.0f,  1.0f, 0.0f),
            D3DXVECTOR3(1.0f, -1.0f, 0.0f),
            D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
            D3DXVECTOR3(-1.0f,  1.0f, 1.0f),
            D3DXVECTOR3(1.0f,  1.0f, 1.0f),
            D3DXVECTOR3(1.0f, -1.0f, 1.0f),
            D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
        };

        D3DXVECTOR3 frustumCenter = D3DXVECTOR3(0, 0, 0);
        for (int i = 0; i < 8; ++i) {
            D3DXVECTOR3 p;
            D3DXVec3TransformCoord(&p, &frustumCorners[i], &invViewProj);
            frustumCorners[i] = D3DXVECTOR3(p.x, p.y, p.z);
            frustumCenter += frustumCorners[i]; 
        }
        frustumCenter /= 8.0f;


        D3DXVECTOR3 shadowCameraPos = frustumCenter + light->GetDirection() * -0.5f;
        LightClass* lightn = new LightClass;
        lightn->setPosition(shadowCameraPos);
        lightn->setLookAt(frustumCenter);
        lightViewMatrix = lightn->GenerateViewMatrix();

       /*model->Render();
        m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, model->GetTexture());
        continue;*/


        // render mesh to each cascade
        for (int cascadeIdx = 0; cascadeIdx < NumCascades; ++cascadeIdx) {
            D3DXVECTOR3 frustumCornersWS[8] =
            {
                D3DXVECTOR3(-1.0f,  1.0f, 0.0f),
                D3DXVECTOR3(1.0f,  1.0f, 0.0f),
                D3DXVECTOR3(1.0f, -1.0f, 0.0f),
                D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
                D3DXVECTOR3(-1.0f,  1.0f, 1.0f),
                D3DXVECTOR3(1.0f,  1.0f, 1.0f),
                D3DXVECTOR3(1.0f, -1.0f, 1.0f),
                D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
            };

            float prevSplitDist = cascadeIdx == 0 ? MinDistance : CascadeSplits[cascadeIdx - 1];
            float splitDist = CascadeSplits[cascadeIdx];

            for (int i = 0; i < 8; ++i) {
                D3DXVec3TransformCoord(&frustumCornersWS[i], &frustumCornersWS[i], &invViewProj);
            }

            for (int i = 0; i < 4; ++i)
            {
                D3DXVECTOR3 cornerRay = frustumCornersWS[i + 4] - frustumCornersWS[i];
                D3DXVECTOR3 nearCornerRay = cornerRay * prevSplitDist;
                D3DXVECTOR3 farCornerRay = cornerRay * splitDist;
                frustumCornersWS[i + 4] = frustumCornersWS[i] + farCornerRay;
                frustumCornersWS[i] = frustumCornersWS[i] + nearCornerRay;
            }

            D3DXVECTOR3 frustumCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
            for (int i = 0; i < 8; ++i) {
                frustumCenter = frustumCenter + frustumCornersWS[i];
            }
            frustumCenter *= 1.0f / 8.0f;

            D3DXVECTOR3 minExtents;
            D3DXVECTOR3 maxExtents;
            D3DXVECTOR3 up;

            up.x = 0.0f;
            up.y = 1.0f;
            up.z = 0.0f;

            // Create a temporary view matrix for the light
            D3DXVECTOR3 lightCameraPos = frustumCenter;
            D3DXVECTOR3 lookAt = frustumCenter - light->GetDirection();
            D3DXMATRIX lightView;
            D3DXMatrixLookAtLH(&lightView, &lightCameraPos, &lookAt, &up);


            // Calculate an AABB around the frustum corners
            D3DXVECTOR3 mins = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
            D3DXVECTOR3 maxes = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
            for (int i = 0; i < 8; ++i)
            {
                D3DXVECTOR3 corner;
                D3DXVec3TransformCoord(&corner, &frustumCornersWS[i], &lightView);

                mins = D3DXVECTOR3(min(mins.x, corner.x), min(mins.y, corner.y), min(mins.z, corner.z));
                maxes = D3DXVECTOR3(max(maxes.x, corner.x), max(maxes.y, corner.y), max(maxes.z, corner.z));
            }

            minExtents = mins;
            maxExtents = maxes;

            // Adjust the min/max to accommodate the filtering size
            float scale = (Options::shadow_width + 5) / static_cast<float>(Options::shadow_width);
            minExtents.x *= scale;
            minExtents.y *= scale;
            maxExtents.x *= scale;
            maxExtents.y *= scale;

            D3DXVECTOR3 cascadeExtents = maxExtents - minExtents;

            // Get position of the shadow camera
            D3DXVECTOR3 shadowCameraPos = frustumCenter + light->GetDirection() * -minExtents.z;

            LightClass* lightn = new LightClass;
            lightn->setPosition(shadowCameraPos);
            lightn->setLookAt(frustumCenter);
            lightViewMatrix = lightn->GenerateViewMatrix();
            lightProjectionMatrix = lightn->GenerateOrthoMatrix(minExtents.x, minExtents.y, maxExtents.x, maxExtents.y, 0.0f, cascadeExtents.z);

            // @todo - add check frustum for object, and enumeration light and after already objects
            model->Render();
            m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, model->GetTexture());
        }

        /*model->Render();
        m_DepthShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, model->GetTexture());*/
        /*CompositeModel* subset = model->getSubset();
        if (subset) {
            for (size_t j = 0; j < subset->getChilds().size(); j++) {
                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                modelSubset->Render();
                m_DepthShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), lightViewMatrix, lightProjectionMatrix, modelSubset->GetTexture());
            }
        }*/
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
    D3DXMATRIX viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;

    modelsAlpha.clear();

    camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    for (size_t i = 0; i < m_modelsRender.size(); i++) {
        if (m_modelsRender[i]->GetIndexCount() == 0) {
            m_modelsRender[i]->Render(camera);
        } else {
            if (m_modelsRender[i]->getAlpha() && !m_modelsRender[i]->isShadow()) {
                modelsAlpha.push_back(m_modelsRender[i]);
            } else {
                ModelClass* model = dynamic_cast<ModelClass*> (m_modelsRender[i]);
                // @todo - its so ugly below
                if ((Options::shadow_enabled && model && model->getLights().size() > 0 && m_modelsShadow.size() > 0) || dynamic_cast<const SkyDomeClass*>(model) != nullptr || dynamic_cast<const TerrainClass*>(model) != nullptr || dynamic_cast<const WaterNode*>(model) != nullptr) {
                    if (dynamic_cast<const TerrainClass*>(model) != nullptr) {
                        TerrainClass* terrain = dynamic_cast<TerrainClass*>(model);
                        terrain->Render(camera, m_modelsShadow.size() ? m_RenderStencilTexture->GetShaderResourceView() : 0);
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
                        LightClass* light = model->getLight(0);
                        light->GenerateViewMatrix();
                        light->GetViewMatrix(lightViewMatrix);
                        //light->GetProjectionMatrix(lightProjectionMatrix);
                        light->GetOrthoMatrix(lightProjectionMatrix);

                        D3DXMATRIX invViewProj, invView;
                        m_RenderStencilTexture->GetProjectionMatrix(invViewProj);
                        
                        //D3DXMatrixTranspose(&invView, &invView);
                        //D3DXMatrixTranspose(&invViewProj, &invViewProj);

                        lightViewMatrix = camera->getWorldMatrix();
                        //D3DXMatrixInverse(&lightViewMatrix, NULL, &lightViewMatrix);
                        D3DXMatrixInverse(&invViewProj, NULL, &invViewProj);

                        invViewProj = invViewProj * lightViewMatrix;
                        
                        D3DXVECTOR3 frustumCorners[8] = {
                            D3DXVECTOR3(-1.0f,  1.0f, 0.0f),
                            D3DXVECTOR3(1.0f,  1.0f, 0.0f),
                            D3DXVECTOR3(1.0f, -1.0f, 0.0f),
                            D3DXVECTOR3(-1.0f, -1.0f, 0.0f),
                            D3DXVECTOR3(-1.0f,  1.0f, 1.0f),
                            D3DXVECTOR3(1.0f,  1.0f, 1.0f),
                            D3DXVECTOR3(1.0f, -1.0f, 1.0f),
                            D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
                        };

                        D3DXVECTOR3 frustumCenter = D3DXVECTOR3(0, 0, 0);
                        for (int i = 0; i < 8; ++i) {
                            D3DXVECTOR3 p;
                            D3DXVec3TransformCoord(&p, &frustumCorners[i], &invViewProj);
                            frustumCorners[i] = D3DXVECTOR3(p.x, p.y, p.z);
                            frustumCenter += frustumCorners[i];
                        }
                        frustumCenter /= 8.0f;
                        D3DXVECTOR3 shadowCameraPos = frustumCenter + light->GetDirection() * -0.5f;
                        LightClass* lightn = new LightClass;
                        lightn->setPosition(shadowCameraPos);
                        lightn->setLookAt(frustumCenter);
                        lightn->GenerateViewMatrix();
                        lightn->GetViewMatrix(lightViewMatrix);


                        if (m_modelsRender[i]->getAlpha()) {
                            m_D3D->TurnOnAlphaBlending();
                        }

                        model->Render();
                        m_ShadowShader->Render(m_D3D->GetDeviceContext(), model->GetIndexCount(), model->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, model->GetTexture(), m_RenderStencilTexture->GetShaderResourceView(), light);

                        /*CompositeModel* subset = model->getSubset();
                        if (subset) {
                            for (size_t j = 0; j < subset->getChilds().size(); j++) {
                                ModelClass* modelSubset = dynamic_cast<ModelClass*>(subset->getChilds()[j]);
                                modelSubset->Render();
                                m_ShadowShader->Render(m_D3D->GetDeviceContext(), modelSubset->GetIndexCount(), modelSubset->GetWorldMatrix(), viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, modelSubset->GetTexture(), m_RenderStencilTexture->GetShaderResourceView(), light);
                            }
                        }*/


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

    Image* image = new Image;
    image->m_baseViewMatrix = camera->getBaseViewMatrix();
    image->m_D3D = m_D3D;
    image->Initialize(Options::screen_width / 4, Options::screen_height / 4, 10, 80);
    image->loadTextureByResource(m_RenderStencilTexture->GetShaderResourceView());
    image->Render();


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


    /*m_D3D->TurnZBufferOff();
    m_D3D->TurnOnAlphaBlending();
    Image* image = new Image();
    image->m_D3D = m_D3D;
    image->Initialize(100, 100, 10, 100);
    image->loadTextureByResource(m_RenderStencilTexture->GetShaderResourceView());
    image->Render();
    m_D3D->TurnOffAlphaBlending();
    m_D3D->TurnZBufferOn();*/
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