#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Frustum = 0;
	m_uiManager = 0;
	m_modelManager = 0;
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

	m_Frustum = new FrustumClass;

	m_uiManager = new UIManager;
	m_uiManager->Initialize(m_D3D, baseViewMatrix);

	m_modelManager = new ModelManager;
	m_modelManager->Initialize(m_D3D);

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
	D3DXMATRIX viewMatrix, projectionMatrix;
	float fogColor = 0.0f;

	m_TriangleCount = 0;
	m_RenderCount = 0;

	m_modelManager->PreRender(m_Camera, m_Frustum);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);
	
	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	m_modelManager->Render(m_Camera, m_Frustum);

	m_uiManager->Render();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}