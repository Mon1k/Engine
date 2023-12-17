#include "systemclass.h"
#include <time.h>
#include <stdio.h>

int Options::shadow_width;
int Options::shadow_height;
float Options::shadow_near;
float Options::shadow_depth;
bool Options::soft_shadow;
bool Options::shadow_enabled;
bool Options::full_screen;
int Options::screen_width;
int Options::screen_height;
unsigned int Options::reflectionLevel;

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Fps = 0;
	m_Timer = 0;
	m_Position = 0;
	m_ResourceManager = 0;

	Options::Init();

	screenWidth = Options::screen_width;
	screenHeight = Options::screen_height;
}


SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::init()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the graphics object. This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) {
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) {
		return false;
	}

	// Create the fps object.
	m_Fps = new FpsClass;
	if (!m_Fps) {
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer) {
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result) {
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the input object. This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if (!m_Input) {
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result) {
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	m_ResourceManager = new ResourceManager;
	if (!m_ResourceManager) {
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if (!m_Position) {
		return false;
	}
	
	return true;
}

void SystemClass::shutdown()
{
	// Release the position object.
	if (m_Position) {
		delete m_Position;
		m_Position = 0;
	}

	if (m_ResourceManager) {
		m_ResourceManager->Shutdown();
		delete m_ResourceManager;
		m_ResourceManager = 0;
	}

	// Release the timer object.
	if (m_Timer) {
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the fps object.
	if (m_Fps) {
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the graphics object.
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input) {
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
}


void SystemClass::run()
{
	MSG msg;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	m_Done = false;
	while (!m_Done) {
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT) {
			m_Done = true;
		} else {
			if (!frame()) {
				m_Done = true;
			} else {
				m_Graphics->Render();
			}
		}

		if (m_Input->IsKeyDown(DIK_F10)) {
			m_Done = true;
		}
		if (m_Input->IsKeyDown(DIK_F12)) {
			createScreenshot();
		}
	}
}


bool SystemClass::frame()
{
	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Input->Frame();

	// frame ui
	m_Graphics->getUiManager()->EventProccesor(m_Input);
	m_Graphics->getUiManager()->frame(m_Timer->GetTime());
	// frame graphic
	m_Graphics->frame(m_Timer);


	// movement camera only unfocussed ui
	if (!m_Graphics->getUiManager()->isFocused()) {
		int mouseX, mouseY;
		D3DXVECTOR3 position, rotation;
		float mouseSensivityX = 18.0f, mouseSensivityY = 18.0f, cameraSensivity = 1.0f;
		int mouseButtonPress = m_Input->getMouseButtonPress();

		m_Input->GetMouseLocation(mouseX, mouseY);

		// rotate camera by keyboard
		m_Position->SetFrameTime(m_Timer->GetTime());

		if (m_Input->IsKeyDown(DIK_LSHIFT)) {
			cameraSensivity *= 2;
		}

		m_Position->setSensivity(cameraSensivity);
		m_Position->MoveLeft(m_Input->IsKeyDown(DIK_LEFTARROW) || m_Input->IsKeyDown(DIK_A));
		m_Position->MoveRight(m_Input->IsKeyDown(DIK_RIGHTARROW) || m_Input->IsKeyDown(DIK_D));
		m_Position->MoveForward(m_Input->IsKeyDown(DIK_UPARROW) || m_Input->IsKeyDown(DIK_W));
		m_Position->MoveBackward(m_Input->IsKeyDown(DIK_DOWNARROW) || m_Input->IsKeyDown(DIK_S));
		m_Position->MoveUpward(m_Input->IsKeyDown(DIK_Z));
		m_Position->MoveDownward(m_Input->IsKeyDown(DIK_X));


		// rotate camera by mouse
		if (mouseButtonPress != MOUSE_BUTTON2) {
			float diffRotationX = m_Input->getMouseDiffPosition().x;
			float diffRotationY = m_Input->getMouseDiffPosition().y;

			m_Position->setSensivity(mouseSensivityY);
			m_Position->LookDownward(diffRotationY > 0.0f);
			m_Position->LookUpward(diffRotationY < 0.0f);

			m_Position->setSensivity(mouseSensivityX);
			m_Position->TurnLeft(diffRotationX < 0.0f);
			m_Position->TurnRight(diffRotationX > 0.0f);
		}

		// Get the current view point rotation.
		position = m_Position->getPosition();
		rotation = m_Position->getRotation();
		m_Graphics->getCamera()->SetRotation(rotation);
		m_Graphics->getCamera()->SetPosition(position);
	}
	
	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::createScreenshot()
{
	printf("create screenshot");
	time_t t;
	struct tm local_time;
	time(&t);
	localtime_s(&local_time, &t);
	
	_wmkdir(L"screenshots");

	char filename[80];
	sprintf(filename, "screenshots/screenshot_%d_%02d_%02d__%02d_%02d_%02d.jpg", local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	
	ID3D11RenderTargetView* m_renderTargetView = m_Graphics->getD3D()->getTargetView();
	ID3D11Resource* pSurface = nullptr;

	m_renderTargetView->GetResource(&pSurface);
	if (pSurface) {
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = this->screenWidth;
		desc.Height = this->screenHeight;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;

		ID3D11Texture2D* pTexture = nullptr;
		m_Graphics->getD3D()->GetDevice()->CreateTexture2D(&desc, nullptr, &pTexture);
		if (pTexture) {
			ID3D11DeviceContext* m_pContext = m_Graphics->getD3D()->GetDeviceContext();
			m_pContext->CopyResource(pTexture, pSurface);
			D3DX11SaveTextureToFileA(m_pContext, pTexture, D3DX11_IFF_JPG, filename);
			pTexture->Release();
		}
		pSurface->Release();
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	posX = posY = 0;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"DS Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (Options::full_screen) {
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	} else {
		// Place the window in the middle of the screen.
		if (screenWidth != this->screenWidth) {
			posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		}
		if (screenHeight != this->screenHeight) {
			posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		}

		// If windowed then set it to 800x600 resolution.
		screenWidth = this->screenWidth;
		screenHeight = this->screenHeight;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    WS_OVERLAPPEDWINDOW,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	RECT rc;
	GetWindowRect(m_hwnd, &rc);
	ClipCursor(&rc);
	ShowCursor(false);
}


void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (Options::full_screen) {
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage) {
		// Check if the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		// Check if the window is being closed.
		case WM_CLOSE:
			PostQuitMessage(0);		
			return 0;

		// All other messages pass to the message handler in the system class.
		default:
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}