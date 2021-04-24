#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Model2 = 0;
	m_ModelPlane = 0;
	m_ModelPlane2 = 0;
	m_Bbox = 0;
	
	m_TextureShader = 0;
	m_MultiTextureShader = 0;
	m_LightMapShader = 0;

	m_LightShader = 0;
	m_Light = 0;
	m_ModelList = 0;
	m_Frustum = 0;

	m_Label = 0;
	m_Label2 = 0;
	m_Button = 0;
	m_Button2 = 0;
	m_Cursor = 0;
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
	if (!m_D3D) {
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition(0.0f, 0.0f, -40.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D, "data/models/midpoly_town_house_01.obj", L"data/textures/T_brightwood_basecolor.png", L"");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model1 object.", L"Error", MB_OK);
		return false;
	}
	

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader) {
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light) {
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);


	m_Model2 = new ModelClass;
	result = m_Model2->Initialize(m_D3D, "data/models/cube.ds", L"data/textures/seafloor.dds", L"");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model2 object", L"Error", MB_OK);
		return false;
	}

	m_ModelPlane = new ModelClass;
	result = m_ModelPlane->Initialize(m_D3D, "data/models/square.ds", L"data/textures/stone01.dds",	L"data/textures/dirt01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model plane object", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane->SetPosition(D3DXVECTOR3(25.0f, 0.0f, -20.0f));
	m_ModelPlane->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));

	m_ModelPlane2 = new ModelClass;
	result = m_ModelPlane2->Initialize(m_D3D, "data/models/square.ds", L"data/textures/stone01.dds", L"data/textures/light01.dds");
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model plane2 object", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane2->SetPosition(D3DXVECTOR3(10.0f, 0.0f, -20.0f));
	m_ModelPlane2->SetScale(D3DXVECTOR3(2.0f, 2.0f, 1.0f));

	// Create the multitexture shader object.
	m_MultiTextureShader = new MultiTextureShaderClass;
	if (!m_MultiTextureShader) {
		return false;
	}

	// Initialize the multitexture shader object.
	result = m_MultiTextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the multitexture shader object", L"Error", MB_OK);
		return false;
	}

	// Create the light map shader object.
	m_LightMapShader = new LightMapShaderClass;
	if (!m_LightMapShader) {
		return false;
	}

	// Initialize the light map shader object.
	result = m_LightMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light map shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the model list object.
	m_ModelList = new ModelListClass;
	if (!m_ModelList) {
		return false;
	}

	// Initialize the model list object.
	result = m_ModelList->Initialize(25);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}


	D3DXVECTOR3 position, size;
	m_Model->GetBoundingBox(position, size);
	m_Bbox = new BBox;
	m_Bbox->CreateBox(m_D3D->GetDevice(), hwnd, position, size);


	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if (!m_Frustum) {
		return false;
	}

	m_Button = new Button;
	m_Button->Initialize(m_D3D, screenWidth, screenHeight, hwnd, L"data/textures/ui/button.png", 76, 28, baseViewMatrix);
	m_Button->Add("New", 10, 10, 1.0f, 1.0f, 1.0f);

	m_Button2 = new Button;
	m_Button2->Initialize(m_D3D, screenWidth, screenHeight, hwnd, L"data/textures/ui/button.png", 76, 28, baseViewMatrix);
	m_Button2->Add("Exit", 10, 40, 1.0f, 0.3f, 0.3f);

	m_Label = new Label;
	m_Label->Initialize(m_D3D, screenWidth, screenHeight, hwnd, 76, 28, baseViewMatrix);
	m_Label->Add("Hello World", 10, 100, 1.0f, 0.3f, 0.3f);

	m_Label2 = new Label;
	m_Label2->Initialize(m_D3D, screenWidth, screenHeight, hwnd, 76, 28, baseViewMatrix);
	m_Label2->Add("", 10, 130, 1.0f, 0.3f, 0.3f);

	m_Cursor = new Cursor;
	m_Cursor->Initialize(m_D3D, screenWidth, screenHeight, hwnd, L"data/textures/ui/cursor.png", 32, 32, baseViewMatrix);
	m_Cursor->Set(screenWidth / 2, screenHeight / 2);

	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_Button) {
		m_Button->Shutdown();
		delete m_Button;
		m_Button = 0;
	}

	if (m_Button2) {
		m_Button2->Shutdown();
		delete m_Button2;
		m_Button2 = 0;
	}

	if (m_Label) {
		m_Label->Shutdown();
		delete m_Label;
		m_Label = 0;
	}

	if (m_Label2) {
		m_Label2->Shutdown();
		delete m_Label2;
		m_Label2 = 0;
	}

	if (m_Cursor) {
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
	}

	// Release the frustum object.
	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the model list object.
	if (m_ModelList) {
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	if (m_Bbox) {
		m_Bbox->Shutdown();
		delete m_Bbox;
		m_Bbox = 0;
	}

	// Release the light object.
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	// Release the light map shader object.
	if (m_LightMapShader) {
		m_LightMapShader->Shutdown();
		delete m_LightMapShader;
		m_LightMapShader = 0;
	}

	// Release the multitexture shader object.
	if (m_MultiTextureShader) {
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = 0;
	}

	// Release the model object.
	if (m_ModelPlane) {
		m_ModelPlane->Shutdown();
		delete m_ModelPlane;
		m_ModelPlane = 0;
	}
	if (m_ModelPlane2) {
		m_ModelPlane2->Shutdown();
		delete m_ModelPlane2;
		m_ModelPlane2 = 0;
	}

	// Release the light shader object.
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D) {
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}

bool GraphicsClass::Frame()
{
	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool renderModel;
	int modelCount, index, triangleCount = 0;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;
	D3DXVECTOR3 position, size;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Get the number of models that will be rendered.
	modelCount = m_ModelList->GetModelCount();

	// Initialize the count of models that have been rendered.
	m_RenderCount = 0;

	// Go through all the models and render them only if they can be seen by the camera view.
	for (index = 0; index < modelCount; index++) {
		// Get the position and color of the sphere model at this index.
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// Set the radius of the sphere to 1.0 since this is already known.
		radius = 1.0f;
		
		// Check if the sphere model is in the view frustum.
		renderModel = m_Frustum->CheckSphere(positionX, positionY, positionZ, radius);

		// If it can be seen then render it, if not skip this model and check the next sphere.
		if (renderModel) {
			// Move the model to the location it should be rendered at.
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ);

			// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			m_Model2->Render(m_D3D->GetDeviceContext());

			// Render the model using the light shader.
			m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
				m_Model2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), color,
				m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());

			// Reset to the original world matrix.
			m_D3D->GetWorldMatrix(worldMatrix);

			// Since this model was rendered then increase the count for this frame.
			m_RenderCount++;
			triangleCount += m_Model2->GetTtriangleCount();
		}
	}


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_Model->Render(m_D3D->GetDeviceContext());
		// Render the model using the light shader.
		m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), m_Model->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		triangleCount += m_Model->GetTtriangleCount();
		m_RenderCount++;
	}


	m_ModelPlane->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_ModelPlane->Render(m_D3D->GetDeviceContext());
		m_MultiTextureShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane->GetIndexCount(), m_ModelPlane->GetWorldMatrix(), viewMatrix, projectionMatrix, m_ModelPlane->GetTextureArray());
		triangleCount += m_ModelPlane->GetTtriangleCount();
		m_RenderCount++;
	}


	m_ModelPlane2->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_ModelPlane2->Render(m_D3D->GetDeviceContext());
		m_LightMapShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane2->GetIndexCount(), m_ModelPlane2->GetWorldMatrix(), viewMatrix, projectionMatrix, m_ModelPlane2->GetTextureArray());
		triangleCount += m_ModelPlane2->GetTtriangleCount();
		m_RenderCount++;
	}


	m_Bbox->Render(m_D3D, viewMatrix);


	// render ui
	char string[128];
	sprintf(string, "Render objects: %d, triangle: %d", m_RenderCount, triangleCount);
	m_Label2->Add(string, 10, 130, 1.0f, 1.0f, 0.5f);

	m_Button->Render();
	m_Button2->Render();
	m_Label->Render();
	m_Label2->Render();
	m_Cursor->Render();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}