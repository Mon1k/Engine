#include "graphicsclass.h"
#include "ui/checkbox.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_uiManager = 0;
	m_modelManager = 0;


	////
	m_TextureShader = 0;

	m_RenderTexture = 0;
	m_DebugWindow = 0;

	m_LightShader = 0;
	m_Light = 0;
	m_Frustum = 0;
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


	// Create the frustum object.
	m_Frustum = new FrustumClass;

	return true;
}


void GraphicsClass::Shutdown()
{
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
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix,orthoMatrix;
	float fogColor = 0.0f;

	m_TriangleCount = 0;
	m_RenderCount = 0;

	m_modelManager->PreRender(m_Camera, m_Frustum);

	// Render the entire scene to the texture first.
	RenderToTexture();


	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);
	
	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	RenderScene();

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_D3D->TurnZBufferOff();
	m_DebugWindow->Render(10, 150);
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, m_Camera->getBaseViewMatrix(),
		orthoMatrix, m_RenderTexture->GetShaderResourceView());
	m_D3D->TurnZBufferOn();

	m_uiManager->Render();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
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

void GraphicsClass::RenderScene()
{
	D3DXMATRIX viewMatrix, projectionMatrix;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	m_modelManager->Render(m_Camera, m_Frustum);
}