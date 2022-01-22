#include "graphicsclass.h"
#include "ui/checkbox.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_uiManager = 0;
	m_modelManager = 0;


	////
	m_ModelPlane7 = 0;
	
	m_TextureShader = 0;
	m_FogShader = 0;
	m_ReflectionShader = 0;
	m_FadeShader = 0;

	// water
	m_GroundModel = 0;
	m_WallModel = 0;
	m_BathModel = 0;
	m_WaterModel = 0;
	m_LightWater = 0;
	m_RefractionTexture = 0;
	m_ReflectionTexture = 0;
	m_LightShaderWater = 0;
	m_RefractionShader = 0;
	m_WaterShader = 0;

	m_RenderTexture = 0;
	m_DebugWindow = 0;
	m_RenderTextureReflection = 0;

	m_LightShader = 0;
	m_Light = 0;
	m_Frustum = 0;

	m_Light1 = 0;
	m_Light2 = 0;

	int size = sizeof(m_Counters) / sizeof(m_Counters[0]);
	for (int i = 0; i < size; i++) {
		m_Counters[i] = 0;
	}

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(NULL, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -80.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	m_Camera->setBaseViewMatrix();


	m_uiManager = new UIManager;
	m_uiManager->Initialize(m_D3D, baseViewMatrix);

	m_modelManager = new ModelManager;
	m_modelManager->Initialize(m_D3D);


	/////////

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	result = m_LightShader->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// direction light
	m_Light = new LightClass;
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);

	// point light
	m_Light1 = new LightClass;
	m_Light1->SetAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	m_Light1->SetDiffuseColor(1.0f, 0.9f, 0.9f, 1.0f);
	m_Light1->SetPosition(-30.0f, 1.0f, -22.0f);
	m_Light2 = new LightClass;
	m_Light2->SetDiffuseColor(1.0f, 0.5f, 0.0f, 1.0f);
	m_Light2->SetAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	m_Light2->SetPosition(-53.0f, 2.0f, -35.0f);


	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	// Create the debug window object.
	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow) {
		return false;
	}

	// Initialize the debug window object.
	float divisor = screenWidth / (float)screenHeight;
	result = m_DebugWindow->Initialize(m_D3D, screenWidth, screenHeight, 100, (int)(100 / divisor));
	if (!result) {
		MessageBox(NULL, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_RenderTextureReflection = new RenderTextureClass;
	// Initialize the render to texture object.
	result = m_RenderTextureReflection->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		return false;
	}


	m_ModelPlane7 = new ModelClass;
	std::vector<std::wstring> textures7 = { L"data/textures/blue01.dds" };
	result = m_ModelPlane7->Initialize(m_D3D, "data/models/floor.ds", textures7);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the model plane 7 object.", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane7->SetScale(D3DXVECTOR3(10.0f, 1.0f, 10.0f));
	m_ModelPlane7->SetPosition(D3DXVECTOR3(-10.0f, -5.0f, -25.0f));





	//// water ////
	// Create the ground model object.
	m_GroundModel = new ModelClass;
	std::vector<std::wstring> texturesGround = {L"data/textures/grass_grass_0066_01.jpg"};
	//result = m_GroundModel->Initialize(m_D3D, "data/models/ground.ds", texturesGround);
	result = m_GroundModel->Initialize(m_D3D, "data/models/plane01.ds", texturesGround);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}
	m_GroundModel->SetPosition(D3DXVECTOR3(-50.0f, -4.9f, -35.0f));
	m_GroundModel->SetScale(D3DXVECTOR3(5.0f, 1.0f, 5.0f));

	// Create the wall model object.
	m_WallModel = new ModelClass;
	std::vector<std::wstring> texturesWall = {L"data/textures/wall01.dds"};
	result = m_WallModel->Initialize(m_D3D, "data/models/wall.ds", texturesWall);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}
	m_WallModel->SetPosition(D3DXVECTOR3(-50.0f, -3.0f, -30.0f));

	// Create the bath model object.
	m_BathModel = new ModelClass;
	std::vector<std::wstring> texturesBath = {L"data/textures/marble01.dds"};
	result = m_BathModel->Initialize(m_D3D, "data/models/bath.ds", texturesBath);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bath model object.", L"Error", MB_OK);
		return false;
	}
	m_BathModel->SetPosition(D3DXVECTOR3(-50.0f, -4.0f, -35.0f));

	// Create the water model object.
	m_WaterModel = new ModelClass;
	std::vector<std::wstring> texturesWater = {L"data/textures/water01.dds"};
	result = m_WaterModel->Initialize(m_D3D, "data/models/water.ds", texturesWater);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the water model object.", L"Error", MB_OK);
		return false;
	}
	m_WaterModel->SetPosition(D3DXVECTOR3(-50.0f, -3.0f, -35.0f));

	// Create the light object.
	m_LightWater = new LightClass;
	m_LightWater->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_LightWater->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_LightWater->SetDirection(0.0f, -1.0f, 1.0f);
	m_LightWater->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_LightWater->SetSpecularPower(32.0f);

	// Create the refraction render to texture object.
	m_RefractionTexture = new RenderTextureClass;
	result = m_RefractionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK);
		return false;
	}
	// Create the reflection render to texture object.
	m_ReflectionTexture = new RenderTextureClass;
	result = m_ReflectionTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the reflection render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShaderWater = new LightShaderClass;
	result = m_LightShaderWater->Initialize(m_D3D->GetDevice());
	if (!result) {
		MessageBox(NULL, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the refraction shader object.
	m_RefractionShader = new RefractionShaderClass;
	result = m_RefractionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the water shader object.
	m_WaterShader = new WaterShaderClass;
	result = m_WaterShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the water shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the height of the water.
	m_waterHeight = 2.75f;
	// Initialize the position of the water.
	m_waterTranslation = 0.0f;
	///////////////


	// Create the fog shader object.
	m_FogShader = new FogShaderClass;
	// Initialize the fog shader object.
	result = m_FogShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the fog shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the reflection shader object.
	m_ReflectionShader = new ReflectionShaderClass;
	// Initialize the reflection shader object.
	result = m_ReflectionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
		return false;
	}



	m_RenderTextureFade = new RenderTextureClass;
	// Initialize the render to texture object.
	result = m_RenderTextureFade->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	m_BitmapFade = new BitmapClass;
	if (!m_BitmapFade) {
		return false;
	}
	// Initialize the bitmap object.
	result = m_BitmapFade->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"data/textures/stone01.dds", screenWidth, screenHeight);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	// Set the fade in time to milliseconds.
	m_fadeInTime = 5000.0f;
	// Initialize the accumulated time to zero milliseconds.
	m_accumulatedTime = 0;
	// Initialize the fade percentage to zero at first so the scene is black.
	m_fadePercentage = 0;
	// Set the fading in effect to not done.
	m_fadeDone = true;
	
	// Create the fade shader object.
	m_FadeShader = new FadeShaderClass;
	// Initialize the fade shader object.
	result = m_FadeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(NULL, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}



	// Create the frustum object.
	m_Frustum = new FrustumClass;

	return true;
}


void GraphicsClass::Shutdown()
{
	//// water ////
	// Release the water shader object.
	if (m_WaterShader) {
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}

	// Release the refraction shader object.
	if (m_RefractionShader) {
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}

	// Release the light shader object.
	if (m_LightShaderWater) {
		m_LightShaderWater->Shutdown();
		delete m_LightShaderWater;
		m_LightShaderWater = 0;
	}

	// Release the reflection render to texture object.
	if (m_ReflectionTexture) {
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// Release the refraction render to texture object.
	if (m_RefractionTexture) {
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}

	// Release the light object.
	if (m_LightWater) {
		delete m_LightWater;
		m_LightWater = 0;
	}

	// Release the water model object.
	if (m_WaterModel) {
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = 0;
	}

	// Release the bath model object.
	if (m_BathModel) {
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = 0;
	}

	// Release the wall model object.
	if (m_WallModel) {
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = 0;
	}

	// Release the ground model object.
	if (m_GroundModel) {
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}
	///////////////

	if (m_uiManager) {
		m_uiManager->Shutdown();
		delete m_uiManager;
		m_uiManager = 0;
	}

	if (m_modelManager) {
		m_modelManager->Shutdown();
		delete m_modelManager;
		m_modelManager = 0;
	}

	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the light object.
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}
	if (m_Light1) {
		delete m_Light1;
		m_Light1 = 0;
	}
	if (m_Light2) {
		delete m_Light2;
		m_Light2 = 0;
	}


	// Release the fade shader object.
	if (m_FadeShader) {
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}
	if (m_BitmapFade) {
		m_BitmapFade->Shutdown();
		delete m_BitmapFade;
		m_BitmapFade = 0;
	}
	if (m_RenderTextureFade) {
		m_RenderTextureFade->Shutdown();
		delete m_RenderTextureFade;
		m_RenderTextureFade = 0;
	}



	// Release the reflection shader object.
	if (m_ReflectionShader) {
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = 0;
	}


	// Release the fog shader object.
	if (m_FogShader)
	{
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = 0;
	}

	// Release the debug window object.
	if (m_DebugWindow) {
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}
	// Release the render to texture object.
	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	if (m_RenderTextureReflection) {
		m_RenderTextureReflection->Shutdown();
		delete m_RenderTextureReflection;
		m_RenderTextureReflection = 0;
	}


	if (m_ModelPlane7) {
		m_ModelPlane7->Shutdown();
		delete m_ModelPlane7;
		m_ModelPlane7 = 0;
	}


	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
}

void GraphicsClass::frame(TimerClass *timer)
{
	float time = timer->GetTime();
	
	if (!m_fadeDone) {
		// Update the accumulated time with the extra frame time addition.
		m_accumulatedTime += time;

		// While the time goes on increase the fade in amount by the time that is passing each frame.
		if (m_accumulatedTime < m_fadeInTime) {
			// Calculate the percentage that the screen should be faded in based on the accumulated time.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		} else {
			// If the fade in time is complete then turn off the fade effect and render the scene normally.
			m_fadeDone = true;
			// Set the percentage to 100%.
			m_fadePercentage = 1.0f;
		}
	}


	// Update the position of the water to simulate motion.
	m_Counters[1] += time;
	if (m_Counters[1] > 50) {
		m_Counters[1] = 0;
		m_waterTranslation += 0.005f;
		if (m_waterTranslation > 1.0f) {
			m_waterTranslation -= 1.0f;
		}
	}
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float fogColor;
	
	fogColor = 0.0f;
	Checkbox* checkbox = dynamic_cast<Checkbox*>(m_uiManager->getById(3));
	if (checkbox->getIsMarked()) {
		fogColor = 0.5f;
	}

	m_TriangleCount = 0;
	m_RenderCount = 0;

	// Render the entire scene to the texture first.
	RenderToTexture();
	RenderToTextureReflection();
	RenderRefractionToTextureWater();
	RenderReflectionToTextureWater();


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);
	
	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	//// render inside ////
	// Render the scene as normal to the back buffer.
	if (m_fadeDone) {
		RenderScene();
	} else {
		RenderToTextureFade();
	}

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_D3D->TurnZBufferOff();
	m_DebugWindow->Render(10, 150);
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, m_Camera->getBaseViewMatrix(),
		orthoMatrix, m_RenderTexture->GetShaderResourceView());
	m_D3D->TurnZBufferOn();

	RenderUI();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

void GraphicsClass::RenderUI()
{
	m_uiManager->Render();
}

void GraphicsClass::RenderToTexture()
{
	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	RenderScene();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void GraphicsClass::RenderToTextureReflection()
{
	D3DXMATRIX reflectionViewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	m_RenderTextureReflection->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTextureReflection->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	m_Camera->RenderReflection(-5.0f);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();
	m_D3D->GetProjectionMatrix(projectionMatrix);
		
	ModelClass* model = dynamic_cast<ModelClass*>(m_modelManager->getById(6));
	model->Render();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void GraphicsClass::RenderToTextureFade()
{
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;

	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Set the render target to be the render to texture.
	m_RenderTextureFade->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTextureFade->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.9f, 0.9f, 0.9f, 0.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	RenderScene();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	m_D3D->TurnZBufferOff();

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_BitmapFade->Render(m_D3D->GetDeviceContext(), 0, 0);

	// Render the bitmap using the fade shader.
	m_FadeShader->Render(m_D3D->GetDeviceContext(), m_BitmapFade->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_RenderTexture->GetShaderResourceView(), m_fadePercentage);

	m_D3D->TurnZBufferOn();
}


void GraphicsClass::RenderRefractionToTextureWater()
{
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX viewMatrix, projectionMatrix;
	D3DXVECTOR3 position = m_WaterModel->GetPosition();

	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = D3DXVECTOR4(0.0f, position.y, 0.0f, m_waterHeight + 0.1f);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_BathModel->Render();
	m_RefractionShader->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), m_BathModel->GetWorldMatrix(), viewMatrix,
		projectionMatrix, m_BathModel->GetTexture(), m_LightWater->GetDirection(),
		m_LightWater->GetAmbientColor(), m_LightWater->GetDiffuseColor(), clipPlane);
	m_TriangleCount += m_BathModel->GetTtriangleCount();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}


void GraphicsClass::RenderReflectionToTextureWater()
{
	D3DXMATRIX reflectionViewMatrix, projectionMatrix;

	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	m_Camera->RenderReflection(m_waterHeight);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object.
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_WallModel->Render();
	std::vector<LightClass*> lights = { m_LightWater };
	m_LightShaderWater->addLights(lights);
	m_LightShaderWater->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), m_WallModel->GetWorldMatrix(), reflectionViewMatrix, projectionMatrix,
		m_WallModel->GetTextureArray(), m_Camera->GetPosition());

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();
}

void GraphicsClass::RenderScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix;
	bool renderModel;
	int index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR3 position, size;
	D3DXVECTOR4 color;
	D3DXVECTOR4 clipPlane;
	float fogStart, fogEnd;

	clipPlane = D3DXVECTOR4(1.0f, 0.0f, 0.0f, -5.0f);
	
	Checkbox* checkbox = dynamic_cast<Checkbox*>(m_uiManager->getById(3));
	if (checkbox->getIsMarked()) {
		// Set the start and end of the fog.
		fogStart = 0.0f;
		fogEnd = 50.0f;
	}

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	m_modelManager->Render(m_Camera, m_Frustum);
	/////


	// m_FogShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(), fogStart, fogEnd);

	m_ModelPlane7->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		reflectionMatrix = m_Camera->GetReflectionViewMatrix();
		m_ModelPlane7->Render();
		m_ReflectionShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane7->GetIndexCount(), m_ModelPlane7->GetWorldMatrix(), viewMatrix,
			projectionMatrix, m_ModelPlane7->GetTexture(), m_RenderTextureReflection->GetShaderResourceView(),
			reflectionMatrix);
		m_TriangleCount += m_ModelPlane7->GetTtriangleCount();
		m_RenderCount++;
	}


	//// water /////
	m_GroundModel->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_GroundModel->Render();
		/*std::vector<LightClass*> lights = { m_LightWater };
		m_LightShaderWater->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), m_GroundModel->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_GroundModel->GetTexture(), m_Camera->GetPosition(), lights);*/
		std::vector<LightClass*> lights = { m_Light1, m_Light2 };
		m_LightShaderWater->addLights(lights);
		m_LightShaderWater->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), m_GroundModel->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_GroundModel->GetTextureArray(), m_Camera->GetPosition());

		m_TriangleCount += m_GroundModel->GetTtriangleCount();
		m_RenderCount++;
	}

	m_WallModel->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_WallModel->Render();
		std::vector<LightClass*> lights = { m_LightWater };
		m_LightShaderWater->addLights(lights);
		m_LightShaderWater->Render(m_D3D->GetDeviceContext(), m_WallModel->GetIndexCount(), m_WallModel->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_WallModel->GetTextureArray(), m_Camera->GetPosition());
		m_TriangleCount += m_WallModel->GetTtriangleCount();
		m_RenderCount++;
	}

	m_BathModel->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_BathModel->Render();
		std::vector<LightClass*> lights = { m_LightWater };
		m_LightShaderWater->addLights(lights);
		m_LightShaderWater->Render(m_D3D->GetDeviceContext(), m_BathModel->GetIndexCount(), m_BathModel->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_BathModel->GetTextureArray(), m_Camera->GetPosition());
		m_TriangleCount += m_BathModel->GetTtriangleCount();
		m_RenderCount++;
	}

	m_WaterModel->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		reflectionMatrix = m_Camera->GetReflectionViewMatrix();
		m_WaterModel->Render();
		m_WaterShader->Render(m_D3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), m_WaterModel->GetWorldMatrix(), viewMatrix,
			projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(),
			m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(),
			m_waterTranslation, 0.02f);
		m_TriangleCount += m_WaterModel->GetTtriangleCount();
		m_RenderCount++;
	}
	////////////////
}