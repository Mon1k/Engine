#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Model2 = 0;
	m_Model3 = 0;
	m_ModelPlane = 0;
	m_ModelPlane2 = 0;
	m_ModelPlane3 = 0;
	m_ModelPlane4 = 0;
	m_Bbox = 0;
	
	m_SpecMapShader = 0;
	m_BumpMapShader = 0;
	m_TextureShader = 0;
	m_MultiTextureShader = 0;
	m_LightMapShader = 0;
	m_AlphaMapShader = 0;
	m_FogShader = 0;
	m_ClipPlaneShader = 0;

	m_RenderTexture = 0;
	m_DebugWindow = 0;

	m_LightShader = 0;
	m_Light = 0;
	m_ModelList = 0;
	m_Frustum = 0;

	m_Label = 0;
	m_Label2 = 0;
	m_Button = 0;
	m_Button2 = 0;
	m_Checkbox = 0;
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
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, -40.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);
	m_Camera->setBaseViewMatrix();

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
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
	if (!result) {
		return false;
	}

	// Create the debug window object.
	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow) {
		return false;
	}

	// Initialize the debug window object.
	float divisor = screenWidth / (float)screenHeight;
	result = m_DebugWindow->Initialize(m_D3D, screenWidth, screenHeight, 100, (int)(100 / divisor));
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}


	// Create the model object.
	m_Model = new ModelClass;
	std::vector<std::wstring> textures1 = { L"data/textures/T_brightwood_basecolor.png" };
	result = m_Model->Initialize(m_D3D, "data/models/midpoly_town_house_01.obj", textures1);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model 1 object.", L"Error", MB_OK);
		return false;
	}

	m_Model2 = new ModelClass;
	std::vector<std::wstring> textures1_1 = { L"data/textures/seafloor.dds" };
	result = m_Model2->Initialize(m_D3D, "data/models/cube.ds", textures1_1);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model 2 object", L"Error", MB_OK);
		return false;
	}

	m_Model3 = new ModelBumpClass;
	std::vector<std::wstring> textures1_2 = { L"data/textures/stone01.dds", L"data/textures/bump01.dds" };
	result = m_Model3->Initialize(m_D3D, "data/models/cube.ds", textures1_2);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model 3 object.", L"Error", MB_OK);
		return false;
	}
	m_Model3->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
	m_Model3->SetPosition(D3DXVECTOR3(5.0f, 0.0f, -20.0f));
	

	m_ModelPlane = new ModelClass;
	std::vector<std::wstring> textures2 = { L"data/textures/stone01.dds", L"data/textures/dirt01.dds" };
	result = m_ModelPlane->Initialize(m_D3D, "data/models/square.ds", textures2);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model plane object", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));
	m_ModelPlane->SetPosition(D3DXVECTOR3(35.0f, 0.0f, -20.0f));
	

	m_ModelPlane2 = new ModelClass;
	std::vector<std::wstring> textures2_1 = { L"data/textures/stone01.dds", L"data/textures/light01.dds" };
	result = m_ModelPlane2->Initialize(m_D3D, "data/models/square.ds", textures2_1);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model plane 2 object", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane2->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));
	m_ModelPlane2->SetPosition(D3DXVECTOR3(25.0f, 0.0f, -20.0f));

	std::vector<std::wstring> textures3 = { L"data/textures/stone01.dds", L"data/textures/dirt01.dds", L"data/textures/alpha01.dds" };
	m_ModelPlane3 = new ModelClass;
	result = m_ModelPlane3->Initialize(m_D3D, "data/models/square.ds", textures3);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model model plane 3 object.", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane3->SetScale(D3DXVECTOR3(5.0f, 5.0f, 1.0f));
	m_ModelPlane3->SetPosition(D3DXVECTOR3(15.0f, 0.0f, -20.0f));

	m_ModelPlane4 = new ModelBumpClass;
	std::vector<std::wstring> textures4 = { L"data/textures/stone02.dds", L"data/textures/bump02.dds", L"data/textures/spec02.dds" };
	result = m_ModelPlane4->Initialize(m_D3D, "data/models/square.ds", textures4);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model plane 4 object.", L"Error", MB_OK);
		return false;
	}
	m_ModelPlane4->SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
	m_ModelPlane4->SetPosition(D3DXVECTOR3(-5.0f, 0.0f, -20.0f));


	// Create the multitexture shader object.
	m_MultiTextureShader = new MultiTextureShaderClass;
	result = m_MultiTextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the multitexture shader object", L"Error", MB_OK);
		return false;
	}

	// Create the light map shader object.
	m_LightMapShader = new LightMapShaderClass;
	result = m_LightMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the alpha map shader object.
	m_AlphaMapShader = new AlphaMapShaderClass;
	result = m_AlphaMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the alpha map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bump map shader object.
	m_BumpMapShader = new BumpMapShaderClass;
	// Initialize the bump map shader object.
	result = m_BumpMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the specular map shader object.
	m_SpecMapShader = new SpecMapShaderClass;
	// Initialize the specular map shader object.
	result = m_SpecMapShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the specular map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the fog shader object.
	m_FogShader = new FogShaderClass;
	// Initialize the fog shader object.
	result = m_FogShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the fog shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the clip plane shader object.
	m_ClipPlaneShader = new ClipPlaneShaderClass;
	// Initialize the clip plane shader object.
	result = m_ClipPlaneShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the clip plane shader object.", L"Error", MB_OK);
		return false;
	}



	// Create the model list object.
	m_ModelList = new ModelListClass;
	result = m_ModelList->Initialize(25);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}


	D3DXVECTOR3 position, size;
	m_ModelPlane->GetBoundingBox(position, size);
	m_Bbox = new BBox;
	m_Bbox->CreateBox(m_D3D->GetDevice(), hwnd, position, size);


	// Create the frustum object.
	m_Frustum = new FrustumClass;

	m_Button = new Button;
	m_Button->Initialize(m_D3D, screenWidth, screenHeight, hwnd, L"data/textures/ui/button.png", 76, 28, baseViewMatrix);
	m_Button->Add("New", 10, 10, 1.0f, 1.0f, 1.0f);

	m_Button2 = new Button;
	m_Button2->Initialize(m_D3D, screenWidth, screenHeight, hwnd, L"data/textures/ui/button.png", 76, 28, baseViewMatrix);
	m_Button2->Add("Exit", 10, 40, 1.0f, 0.3f, 0.3f);

	m_Checkbox = new Checkbox;
	m_Checkbox->Initialize(m_D3D, screenWidth, screenHeight, hwnd, L"data/textures/ui/checkbox.png", L"data/textures/ui/checkbox_marked.png", 18, 18, baseViewMatrix);
	m_Checkbox->Add("Fog", 10, 70, 1.0f, 1.0f, 1.0f);

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

	if (m_Checkbox) {
		m_Checkbox->Shutdown();
		delete m_Checkbox;
		m_Checkbox = 0;
	}

	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = 0;
	}

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



	// Release the clip plane shader object.
	if (m_ClipPlaneShader) {
		m_ClipPlaneShader->Shutdown();
		delete m_ClipPlaneShader;
		m_ClipPlaneShader = 0;
	}

	// Release the fog shader object.
	if (m_FogShader)
	{
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = 0;
	}

	// Release the specular map shader object.
	if (m_SpecMapShader) {
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = 0;
	}

	// Release the bump map shader object.
	if (m_BumpMapShader) {
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = 0;
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

	// Release the alpha map shader object.
	if (m_AlphaMapShader) {
		m_AlphaMapShader->Shutdown();
		delete m_AlphaMapShader;
		m_AlphaMapShader = 0;
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
	if (m_ModelPlane3) {
		m_ModelPlane3->Shutdown();
		delete m_ModelPlane3;
		m_ModelPlane3 = 0;
	}
	if (m_ModelPlane4) {
		m_ModelPlane4->Shutdown();
		delete m_ModelPlane4;
		m_ModelPlane4 = 0;
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

	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Model3) {
		m_Model3->Shutdown();
		delete m_Model3;
		m_Model3 = 0;
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
	int modelCount, index;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;
	D3DXVECTOR4 clipPlane;
	D3DXVECTOR3 position, size;
	bool result;
	float fogColor, fogStart, fogEnd;

	fogColor = 0.0f;
	clipPlane = D3DXVECTOR4(1.0f, 0.0f, 0.0f, -5.0f);

	if (m_Checkbox->getIsMarked()) {
		// Set the color of the fog to grey.
		fogColor = 0.5f;
		// Set the start and end of the fog.
		fogStart = 0.0f;
		fogEnd = 50.0f;
	}

	m_TriangleCount = 0;
	m_RenderCount = 0;

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Render the entire scene to the texture first.
	RenderToTexture();

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(fogColor, fogColor, fogColor, 1.0f);

	
	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);


	//// render inside ////

	// Render the scene as normal to the back buffer.
	RenderScene();

	// Go through all the models and render them only if they can be seen by the camera view.
	modelCount = m_ModelList->GetModelCount();
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
			m_Model2->Render();

			// Render the model using the light shader.
			if (m_Checkbox->getIsMarked()) {
				m_FogShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model2->GetTexture(), fogStart, fogEnd);
			} else {
				m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
					m_Model2->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), color,
					m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
			}

			// Reset to the original world matrix.
			m_D3D->GetWorldMatrix(worldMatrix);

			// Since this model was rendered then increase the count for this frame.
			m_RenderCount++;
			m_TriangleCount += m_Model2->GetTtriangleCount();
		}
	}


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model3->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_Model3->Render();
		m_BumpMapShader->Render(m_D3D->GetDeviceContext(), m_Model3->GetIndexCount(), m_Model3->GetWorldMatrix(), viewMatrix, projectionMatrix, m_Model3->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
		/*m_ClipPlaneShader->Render(m_D3D->GetDeviceContext(), m_Model3->GetIndexCount(), m_Model3->GetWorldMatrix(), viewMatrix,
			projectionMatrix, m_Model3->GetTexture(), clipPlane);*/
		m_TriangleCount += m_Model3->GetTtriangleCount();
		m_RenderCount++;
	}


	m_ModelPlane->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_ModelPlane->Render();
		m_MultiTextureShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane->GetIndexCount(), m_ModelPlane->GetWorldMatrix(), viewMatrix, projectionMatrix, m_ModelPlane->GetTextureArray());
		m_TriangleCount += m_ModelPlane->GetTtriangleCount();
		m_RenderCount++;
	}


	m_ModelPlane2->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_ModelPlane2->Render();
		m_LightMapShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane2->GetIndexCount(), m_ModelPlane2->GetWorldMatrix(), viewMatrix, projectionMatrix, m_ModelPlane2->GetTextureArray());
		m_TriangleCount += m_ModelPlane2->GetTtriangleCount();
		m_RenderCount++;
	}

	m_ModelPlane3->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_ModelPlane3->Render();
		m_AlphaMapShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane3->GetIndexCount(), m_ModelPlane3->GetWorldMatrix(), viewMatrix, projectionMatrix, m_ModelPlane3->GetTextureArray());
		m_TriangleCount += m_ModelPlane3->GetTtriangleCount();
		m_RenderCount++;
	}

	m_ModelPlane4->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_ModelPlane4->Render();
		m_SpecMapShader->Render(m_D3D->GetDeviceContext(), m_ModelPlane4->GetIndexCount(), m_ModelPlane4->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_ModelPlane4->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		m_TriangleCount += m_ModelPlane4->GetTtriangleCount();
		m_RenderCount++;
	}

	m_Bbox->Render(m_D3D, viewMatrix);

	// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_D3D->TurnZBufferOff();
	m_DebugWindow->Render(10, 150);
	m_TextureShader->Render(m_D3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(), worldMatrix, m_Camera->getBaseViewMatrix(),
		orthoMatrix, m_RenderTexture->GetShaderResourceView());
	m_D3D->TurnZBufferOn();


	// render ui
	char string[128];
	sprintf(string, "Render objects: %d, triangles: %d", m_RenderCount, m_TriangleCount);
	m_Label2->Add(string, 10, 130, 1.0f, 1.0f, 0.5f);

	m_Button->Render();
	m_Button2->Render();
	m_Label->Render();
	m_Label2->Render();
	m_Checkbox->Render();
	m_Cursor->Render();
	

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::RenderToTexture()
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene();
	if (!result) {
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenderScene()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	D3DXVECTOR3 position, size;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_Model->GetBoundingBox(position, size);
	if (m_Frustum->CheckRectangle(position, size)) {
		m_Model->Render();
		m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), m_Model->GetWorldMatrix(), viewMatrix, projectionMatrix,
			m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
		m_TriangleCount += m_Model->GetTtriangleCount();
		m_RenderCount++;
	}

	return true;
}